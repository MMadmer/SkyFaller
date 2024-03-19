// Sky Faller. All rights reserved.


#include "Objects/SFPlatformMesh.h"

ASFPlatformMesh::ASFPlatformMesh()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	SetRootComponent(StaticMesh);
}

void ASFPlatformMesh::BeginPlay()
{
	Super::BeginPlay();
}
