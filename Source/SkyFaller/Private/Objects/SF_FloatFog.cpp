// Sky Faller. All rights reserved.


#include "Objects/SF_FloatFog.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"


DEFINE_LOG_CATEGORY_STATIC(LogFog, All, All)

ASF_FloatFog::ASF_FloatFog()
{
	PrimaryActorTick.bCanEverTick = false;

	FogMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	SetRootComponent(FogMesh);
}

void ASF_FloatFog::BeginPlay()
{
	Super::BeginPlay();

	FogMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void ASF_FloatFog::Mover(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!GetWorld()) return;
	const auto Player = Cast<ACharacter>(OtherActor);
	if (!Player) return;

	FVector PlayerLocation = Player->GetActorLocation();
	SetActorLocation(FVector(PlayerLocation.X, PlayerLocation.Y, -1500.0f));
}
