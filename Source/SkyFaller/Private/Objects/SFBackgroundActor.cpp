// Sky Faller. All rights reserved.


#include "Objects/SFBackgroundActor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogBackground, All, All)

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

	if (!bDespawned)
	{
		bSpawned ? Spawner(DeltaTime) : Mover(DeltaTime);
	}
	else
	{
		Despawner(DeltaTime);
	}
}

void ASFBackgroundActor::SetTemplate()
{
	int32 CurrentIndex;
	TArray<UStaticMesh*> LayerAssets = BackLayers[Layer].Assets;

	CurrentIndex = FMath::RandRange(0, LayerAssets.Num() - 1);

	BackMesh->SetStaticMesh(LayerAssets[CurrentIndex]);
}

void ASFBackgroundActor::Spawner(float DeltaTime)
{
	if (GetActorLocation().Z >= ParentZ)
	{
		bSpawned = false;
		return;
	}

	SetActorLocation(GetActorLocation() + FVector(0.0f, 0.0f, BackLayers[Layer].SpawnSpeed * DeltaTime));
}

void ASFBackgroundActor::Despawner(float DeltaTime)
{
	if (!GetWorld()) return;

	if (GetActorLocation().Z <= GetWorld()->GetWorldSettings()->KillZ)
	{
		TArray<AActor*> Actors;
		GetAttachedActors(Actors);
		for (const auto Actor : Actors)
		{
			Actor->Destroy();
		}
		Destroy();
	}
	else
	{
		SetActorLocation(GetActorLocation() - FVector(0.0f, 0.0f, BackLayers[Layer].DespawnSpeed * DeltaTime));
	}
}

void ASFBackgroundActor::Mover(float DeltaTime)
{
	if (!GetWorld()) return;

	// Vertical moving
	float Time = GetWorld()->GetTimeSeconds() + LocalTime;
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = ParentZ + BackLayers[Layer].Amplitude * FMath::Sin(BackLayers[Layer].Frequency * Time);

	// Set new location
	SetActorLocation(NewLocation);
}

ASFBackgroundActor* ASFBackgroundActor::SpawnNext(TSubclassOf<ASFBackgroundActor> BackgroundClass)
{
	if (!GetWorld()) return nullptr;
	
	FTransform NewTransform;
	ASFBackgroundActor* NewActor = GetWorld()->SpawnActorDeferred<ASFBackgroundActor>(BackgroundClass, NewTransform);

	FVector NewLocation = GetActorLocation();
	NewLocation.X += BackLayers[Layer].BetweenX;
	NewLocation.Z = ParentZ + BackLayers[Layer].SpawnHeight;
	NewTransform.SetLocation(NewLocation);

	NewActor->Layer = Layer;
	NewActor->ParentZ = ParentZ;

	NewActor->FinishSpawning(NewTransform);

	return NewActor;
}
