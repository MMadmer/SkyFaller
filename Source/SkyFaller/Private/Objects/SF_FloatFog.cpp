// Sky Faller. All rights reserved.


#include "Objects/SF_FloatFog.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"


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

	FogMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
}
