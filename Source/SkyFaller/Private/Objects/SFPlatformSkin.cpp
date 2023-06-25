// Sky Faller. All rights reserved.


#include "Objects/SFPlatformSkin.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASFPlatformSkin::ASFPlatformSkin()
{
	PrimaryActorTick.bCanEverTick = false;

	SkinMesh = CreateDefaultSubobject<UStaticMeshComponent>("SkinMesh");
	SetRootComponent(SkinMesh);
}

void ASFPlatformSkin::BeginPlay()
{
	Super::BeginPlay();
}
