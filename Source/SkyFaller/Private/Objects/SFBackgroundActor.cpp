// Sky Faller. All rights reserved.


#include "Objects/SFBackgroundActor.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/WorldSettings.h"

ASFBackgroundActor::ASFBackgroundActor()
{
	PrimaryActorTick.bCanEverTick = true;

	BackMesh = CreateDefaultSubobject<UStaticMeshComponent>("BackMesh");
	SetRootComponent(BackMesh);
	BackMesh->CastShadow = false;
}

void ASFBackgroundActor::BeginPlay()
{
	Super::BeginPlay();

	LocalTime = static_cast<float>(FMath::RandHelper(10));
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
	TArray<UStaticMesh*> LayerAssets = BackLayers[Layer].Assets;

	const int32 CurrentIndex = FMath::RandRange(0, LayerAssets.Num() - 1);

	BackMesh->SetStaticMesh(LayerAssets[CurrentIndex]);
}

void ASFBackgroundActor::Spawner(const float DeltaTime)
{
	if (GetActorLocation().Z >= ParentZ)
	{
		bSpawned = false;
		return;
	}

	SetActorLocation(GetActorLocation() + FVector(0.0f, 0.0f, BackLayers[Layer].SpawnSpeed * DeltaTime));
}

void ASFBackgroundActor::Despawner(const float DeltaTime)
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
	const float Time = GetWorld()->GetTimeSeconds() + LocalTime;
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = ParentZ + BackLayers[Layer].Amplitude * FMath::Sin(BackLayers[Layer].Frequency * Time);

	// Set new location
	SetActorLocation(NewLocation);
}

ASFBackgroundActor* ASFBackgroundActor::SpawnNext(TSubclassOf<ASFBackgroundActor> BackgroundClass, bool bFront)
{
	if (!GetWorld()) return nullptr;

	FTransform NewTransform;
	ASFBackgroundActor* NewActor = GetWorld()->SpawnActorDeferred<ASFBackgroundActor>(BackgroundClass, NewTransform);

	NewActor->Layer = Layer;
	NewActor->ParentZ = BackLayers[Layer].DistZ + FMath::RandRange(-BackLayers[Layer].OffsetZ,
	                                                               BackLayers[Layer].OffsetZ);

	FVector NewLocation = GetActorLocation();
	NewLocation.X += FMath::RandRange(-BackLayers[Layer].OffsetX, BackLayers[Layer].OffsetX) + (bFront
			? BackLayers[Layer].BetweenX
			: -BackLayers[Layer].BetweenX);
	NewLocation.Y = FMath::Sign(NewLocation.Y) * (BackLayers[Layer].DistY + FMath::RandRange(
		-BackLayers[Layer].OffsetY, BackLayers[Layer].OffsetY));
	NewLocation.Z = NewActor->ParentZ + BackLayers[Layer].SpawnHeight;
	NewTransform.SetLocation(NewLocation);

	NewActor->FinishSpawning(NewTransform);

	return NewActor;
}
