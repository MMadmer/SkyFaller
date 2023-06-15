// Sky Faller. All rights reserved.


#include "Player/Weapon/SFArrow.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "CoreTypes.h"
#include "GameFramework/Character.h"

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
}

void ASFArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PhysicsFalling();
}

void ASFArrow::BeginPlay()
{
	Super::BeginPlay();

	MovementComponent->Velocity = ShotDirection * MovementComponent->InitialSpeed;
	ArrowMesh->IgnoreActorWhenMoving(GetOwner(), true);
	SetLifeSpan(30.0f);

	ArrowMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	ArrowMesh->OnComponentHit.AddDynamic(this, &ASFArrow::ConnectToActor);
}

void ASFArrow::ConnectToActor(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bAttached) return;
	bAttached = true;

	// Penetration imitation
	PercentagePenetration = 1 - PercentagePenetration;
	float ArrowLenght = (GetComponentsBoundingBox().GetExtent() * 2.0f).Size();
	float PenetrationOffset = ArrowLenght * PercentagePenetration;
	float PercentageOffset = PenetrationOffset / MovementComponent->Velocity.Size();
	// UE_LOG(LogArrow, Display, TEXT("Percentage offset: %f"), ArrowLenght);
	SetActorLocation(GetActorLocation() - MovementComponent->Velocity * PercentageOffset);

	// Attach
	AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);
}

void ASFArrow::PhysicsFalling()
{
	if (bAttached) return;

	// Physics rotation
	FRotator NewRotation = MovementComponent->Velocity.Rotation();
	NewRotation.Pitch -= 90.0f;
	SetActorRotation(NewRotation);
}
