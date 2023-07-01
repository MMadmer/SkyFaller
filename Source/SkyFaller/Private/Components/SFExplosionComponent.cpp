// Sky Faller. All rights reserved.


#include "Components/SFExplosionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CollisionQueryParams.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Engine/World.h"
#include "Player/BaseCharacter.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogExplosionComp, All, All)

USFExplosionComponent::USFExplosionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USFExplosionComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void USFExplosionComponent::Explode()
{
	if (!GetWorld()) return;

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

	RadialDamage();

	// Niagara
	if (NiagaraSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
	}

	// Explosion sound
	UAudioComponent* AudioComponent = NewObject<UAudioComponent>(this);
	USoundAttenuation* AttenuationSettings = NewObject<USoundAttenuation>(this);
	if (AudioComponent && ExplosionSound && AttenuationSettings)
	{
		// Set sound radius
		AttenuationSettings->Attenuation.bAttenuate = true;
		AttenuationSettings->Attenuation.AttenuationShape = EAttenuationShape::Sphere;
		AttenuationSettings->Attenuation.AttenuationShapeExtents = FVector(SoundRadius);

		// Play sound
		AudioComponent->SetSound(ExplosionSound);
		AudioComponent->bAllowSpatialization = true;
		AudioComponent->bAutoDestroy = true;
		AudioComponent->AttenuationSettings = AttenuationSettings;
		AudioComponent->SetWorldLocation(GetOwner()->GetActorLocation());
		AudioComponent->Play();
	}
	
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
