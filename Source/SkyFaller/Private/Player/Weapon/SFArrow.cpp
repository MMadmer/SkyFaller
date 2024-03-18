// Sky Faller. All rights reserved.


#include "Player/Weapon/SFArrow.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "CoreTypes.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "NiagaraComponent.h"
#include "Player/BaseCharacter.h"
#include "Objects/SFTrap.h"

DEFINE_LOG_CATEGORY_STATIC(LogArrow, All, All)

ASFArrow::ASFArrow()
{
	PrimaryActorTick.bCanEverTick = true;

	ArrowMesh = CreateDefaultSubobject<USkeletalMeshComponent>("ArrowMesh");
	ArrowMesh->bReturnMaterialOnMove = true;
	SetRootComponent(ArrowMesh);


	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent");
	MovementComponent->InitialSpeed = 2000.0f;
	MovementComponent->ProjectileGravityScale = 1.0f;

	HitSound = nullptr;
}

void ASFArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PhysicsFalling();
}

FVector ASFArrow::GetVelocity() const
{
	return MovementComponent->Velocity;
}

void ASFArrow::BeginPlay()
{
	Super::BeginPlay();

	const auto Player = Cast<ABaseCharacter>(GetOwner());
	if (!Player) return;

	MovementComponent->Velocity = ShotDirection * MovementComponent->InitialSpeed + Player->GetVelocity();

	ArrowMesh->IgnoreActorWhenMoving(GetOwner(), true);
	SetLifeSpan(30.0f);

	ArrowMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	ArrowMesh->SetMassOverrideInKg(NAME_None, Mass);

	ArrowMesh->OnComponentHit.AddDynamic(this, &ASFArrow::OnHit);
}

void ASFArrow::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                     FVector NormalImpulse, const FHitResult& Hit)
{
	if (bAttached) return;
	bAttached = true;

	// Penetration imitation
	PercentagePenetration = 1 - PercentagePenetration;
	const float ArrowLength = (GetComponentsBoundingBox().GetExtent() * 2.0f).Size();
	const float PenetrationOffset = ArrowLength * PercentagePenetration;
	const float PercentageOffset = PenetrationOffset / MovementComponent->Velocity.Size();
	// UE_LOG(LogArrow, Display, TEXT("Percentage offset: %f"), ArrowLength);
	SetActorLocation(GetActorLocation() - MovementComponent->Velocity * PercentageOffset);

	// Attach
	AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);

	// Physics
	if (OtherComp->IsSimulatingPhysics())
	{
		OtherComp->CalculateMass();
		const FVector Force = MovementComponent->Velocity * ArrowMesh->GetMass() * ImpactForceMultiplier;
		OtherComp->AddForceAtLocation(Force, Hit.ImpactPoint);
	}

	if (!Cast<ASFTrap>(OtherActor))
	{
		// Hit sound
		UAudioComponent* AudioComponent = NewObject<UAudioComponent>(this);
		USoundAttenuation* AttenuationSettings = NewObject<USoundAttenuation>(this);
		if (!(AudioComponent && HitSound && AttenuationSettings)) return;

		// Set sound radius
		AttenuationSettings->Attenuation.bAttenuate = true;
		AttenuationSettings->Attenuation.AttenuationShape = EAttenuationShape::Sphere;
		AttenuationSettings->Attenuation.AttenuationShapeExtents = FVector(HitSoundRadius);

		// Play sound
		AudioComponent->SetSound(HitSound);
		AudioComponent->bAllowSpatialization = true;
		AudioComponent->bAutoDestroy = true;
		AudioComponent->AttenuationSettings = AttenuationSettings;
		AudioComponent->SetWorldLocation(Hit.ImpactPoint);
		AudioComponent->Play();
	}

	// Delete niagara FX
	UNiagaraComponent* NiagaraComponent = FindComponentByClass<UNiagaraComponent>();
	if (NiagaraComponent || !GetWorld()) NiagaraComponent->Deactivate();
}

void ASFArrow::PhysicsFalling()
{
	if (bAttached) return;

	// Physics rotation
	FRotator NewRotation = MovementComponent->Velocity.Rotation();
	NewRotation.Pitch -= 90.0f;
	SetActorRotation(NewRotation);
}
