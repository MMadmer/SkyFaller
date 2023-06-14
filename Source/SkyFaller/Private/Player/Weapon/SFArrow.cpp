// Sky Faller. All rights reserved.


#include "Player/Weapon/SFArrow.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogArrow, All, All)

ASFArrow::ASFArrow()
{
	PrimaryActorTick.bCanEverTick = false;

	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>("SkeletalMesh");
	ArrowMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ArrowMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	ArrowMesh->bReturnMaterialOnMove = true;
	SetRootComponent(ArrowMesh);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent");
	MovementComponent->InitialSpeed = 2000.0f;
	MovementComponent->ProjectileGravityScale = 1.0f;
}

void ASFArrow::BeginPlay()
{
	Super::BeginPlay();
	
	// UE_LOG(LogArrow, Display, TEXT("Spawned"));

	MovementComponent->Velocity = ShotDirection * MovementComponent->InitialSpeed;
	ArrowMesh->IgnoreActorWhenMoving(GetOwner(), true);
	SetLifeSpan(5.0f);
}
