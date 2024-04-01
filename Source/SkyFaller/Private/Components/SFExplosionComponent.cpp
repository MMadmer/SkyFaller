// Sky Faller. All rights reserved.


#include "Components/SFExplosionComponent.h"
#include "CollisionQueryParams.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Engine/World.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogExplosionComp, All, All)

USFExplosionComponent::USFExplosionComponent() : NiagaraSystem(nullptr), ExplosionSound(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>("RadialForce");
	RadialForceComponent->Radius = OuterRad;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>("DamageSphere");
	SphereComponent->SetSphereRadius(OuterRad);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Overlap);

	if (!GetOwner()) return;

	RadialForceComponent->SetupAttachment(GetOwner()->GetRootComponent());
	SphereComponent->SetupAttachment(GetOwner()->GetRootComponent());
}

void USFExplosionComponent::BeginPlay()
{
	Super::BeginPlay();

	RadialForceComponent->Radius = 0.0f;
	
	SphereComponent->SetSphereRadius(0.0f);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &USFExplosionComponent::OnExplosionOverlap);
}

void USFExplosionComponent::Explode(const FVector& ImpactLocation)
{
	if (!GetWorld()) return;

	TArray<AActor*> AttachedActors;
	GetOwner()->GetAttachedActors(AttachedActors);
	for (const auto& Actor : AttachedActors)
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
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, ImpactLocation);
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
}

void USFExplosionComponent::RadialDamage() const
{
	SphereComponent->SetSphereRadius(OuterRad);
	SphereComponent->SetSphereRadius(0.0f, false);

	RadialForceComponent->Radius = OuterRad;
	RadialForceComponent->ForceStrength = ExplosionForce;
	RadialForceComponent->ImpulseStrength = ExplosionImpulse;
	RadialForceComponent->FireImpulse();
	RadialForceComponent->Radius = 0.0f;
}

void USFExplosionComponent::OnExplosionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                               const FHitResult& SweepResult)
{
	const float Distance = (OtherActor->GetActorLocation() - GetOwner()->GetActorLocation()).Size() - InnerRad;

	const float DamageToTake = FMath::Clamp(
		Damage * (MinDamage + (1 - MinDamage) * (1 - Distance / (OuterRad - InnerRad))),
		Damage * MinDamage, Damage); // Modified parabolic interpolation

	OtherActor->TakeDamage(DamageToTake, FDamageEvent::FDamageEvent(), nullptr, GetOwner());
}
