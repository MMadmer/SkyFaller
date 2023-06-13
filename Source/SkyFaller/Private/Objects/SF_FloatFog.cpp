// Sky Faller. All rights reserved.


#include "Objects/SF_FloatFog.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"


DEFINE_LOG_CATEGORY_STATIC(LogFog, All, All)

ASF_FloatFog::ASF_FloatFog()
{
	PrimaryActorTick.bCanEverTick = true;

	FogMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	SetRootComponent(FogMesh);
}

void ASF_FloatFog::BeginPlay()
{
	Super::BeginPlay();

	FogMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void ASF_FloatFog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Mover();
}

FVector ASF_FloatFog::GetPlayerLocation() const
{
	FVector PlayerLoaction(0.0f, 0.0f, -1500.0f);

	if (APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		PlayerLoaction = Player->GetActorLocation();
	}

	return PlayerLoaction;
}

void ASF_FloatFog::Mover()
{
	if (!GetWorld()) return;
	FVector PlayerLocation = GetPlayerLocation();
	if (PlayerLocation == FVector(0.0f, 0.0f, -1500.0f)) return;
	SetActorLocation(FVector(PlayerLocation.X, PlayerLocation.Y, -1500.0f));
}
