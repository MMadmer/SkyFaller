// Sky Faller. All rights reserved.


#include "Objects/SFBackgroundActor.h"
#include "Components/StaticMeshComponent.h"

ASFBackgroundActor::ASFBackgroundActor()
{
	PrimaryActorTick.bCanEverTick = true;

	BackMesh = CreateDefaultSubobject<UStaticMeshComponent>("BackMesh");
	SetRootComponent(BackMesh);
}

void ASFBackgroundActor::BeginPlay()
{
	Super::BeginPlay();

	LocalTime = (float)FMath::RandHelper(10);
	SetTemplate();
}

void ASFBackgroundActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Mover(DeltaTime);
}

void ASFBackgroundActor::SetTemplate()
{
	int32 CurrentIndex;
	TArray<UStaticMesh*> LayerAssets = BackLayers[Layer].Assets;

	CurrentIndex = FMath::RandRange(0, LayerAssets.Num() - 1);

	BackMesh->SetStaticMesh(LayerAssets[CurrentIndex]);
}

void ASFBackgroundActor::Mover(float DeltaTime)
{
	if (!GetWorld()) return;

	// Vertical moving
	float Time = GetWorld()->GetTimeSeconds() + LocalTime;
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = ParentZ + Amplitude * FMath::Sin(Frequency * Time);

	// Set new location
	SetActorLocation(NewLocation);
}
