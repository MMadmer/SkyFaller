// Sky Faller. All rights reserved.


#include "Objects/SFTrap.h"
#include "Components/StaticMeshComponent.h"

ASFTrap::ASFTrap()
{
	PrimaryActorTick.bCanEverTick = false;

	TrapMesh = CreateDefaultSubobject<UStaticMeshComponent>("TrapMesh");
	SetRootComponent(TrapMesh);
}

void ASFTrap::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASFTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
