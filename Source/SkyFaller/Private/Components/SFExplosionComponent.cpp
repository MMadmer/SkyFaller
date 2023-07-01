// Sky Faller. All rights reserved.


#include "Components/SFExplosionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CollisionQueryParams.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Player/BaseCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogExplosionComp, All, All)

USFExplosionComponent::USFExplosionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USFExplosionComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void USFExplosionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetWorld()) return;

	DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), InnerRad, 16, FColor::Red);
	DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), OuterRad, 16, FColor::Blue);
}

void USFExplosionComponent::Explode()
{
	TArray<AActor*> AttachedActors;
	GetOwner()->GetAttachedActors(AttachedActors);
	for (auto Actor : AttachedActors)
	{
		Actor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
		if (PrimitiveComponent)
		{
			PrimitiveComponent->SetSimulatePhysics(true);
		}
	}

	/*if (!GetWorld()) return;
	UGameplayStatics::ApplyRadialDamageWithFalloff(
		GetWorld(), 
		Damage, 
		Damage * MinDamage, 
		GetOwner()->GetActorLocation(), 
		InnerRad, 
		OuterRad, 
		DamageFalloff, 
		nullptr, 
		{}, 
		GetOwner(), 
		nullptr,
		ECollisionChannel::ECC_WorldStatic);*/

	RadialDamage();
	
	RadialForceComponent = NewObject<URadialForceComponent>(GetOwner());
	RadialForceComponent->RegisterComponent();
	RadialForceComponent->SetWorldLocation(GetOwner()->GetActorLocation());
	RadialForceComponent->Radius = OuterRad;
	RadialForceComponent->ForceStrength = ExplosionForce;
	RadialForceComponent->ImpulseStrength = ExplosionImpulse;

	RadialForceComponent->FireImpulse();
}

void USFExplosionComponent::RadialDamage()
{
	SphereComponent = NewObject<USphereComponent>(GetOwner());
	SphereComponent->RegisterComponent();
	SphereComponent->SetWorldLocation(GetOwner()->GetActorLocation());
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	SphereComponent->SetSphereRadius(0.0f);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &USFExplosionComponent::OnExplosionOverlap);
	SphereComponent->SetSphereRadius(OuterRad);
	SphereComponent->DestroyComponent();
}

void USFExplosionComponent::OnExplosionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const auto Player = Cast<ABaseCharacter>(OtherActor);
	if (!Player) return;

	float Distance = (Player->GetActorLocation() - GetOwner()->GetActorLocation()).Size() - InnerRad;
	if (Distance <= 0.0f) Player->TakeDamage(Damage, FDamageEvent::FDamageEvent(), nullptr, GetOwner());
	else
	{
		float DamageToTake = Damage * (MinDamage + (1 - MinDamage) * (1 - Distance / (OuterRad - InnerRad))); // Parabolic interpolation

		Player->TakeDamage(DamageToTake, FDamageEvent::FDamageEvent(), nullptr, GetOwner());
	}
}
