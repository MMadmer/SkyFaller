// Sky Faller. All rights reserved.


#include "Player/Weapon/SFArrow.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "CoreTypes.h"

DEFINE_LOG_CATEGORY_STATIC(LogArrow, All, All)

ASFArrow::ASFArrow()
{
	PrimaryActorTick.bCanEverTick = false;

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


}

void ASFArrow::BeginPlay()
{
	Super::BeginPlay();

	MovementComponent->Velocity = ShotDirection * MovementComponent->InitialSpeed;
	ArrowMesh->IgnoreActorWhenMoving(GetOwner(), true);
	SetLifeSpan(30.0f);

	ArrowMesh->OnComponentHit.AddDynamic(this, &ASFArrow::ConnectToActor);
}

void ASFArrow::ConnectToActor(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);
}
