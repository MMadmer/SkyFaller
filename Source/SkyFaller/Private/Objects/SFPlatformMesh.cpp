// Sky Faller. All rights reserved.


#include "Objects/SFPlatformMesh.h"

#include "Engine/StaticMeshActor.h"

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

	SpawnAllObjects();
}

void ASFPlatformMesh::SpawnAllObjects()
{
	// Get all scene components
	TArray<UActorComponent*> SceneComponents;
	GetComponents(USceneComponent::StaticClass(), SceneComponents);

	for (const auto& ActorComponent : SceneComponents)
	{
		// Get scene component
		const auto& SceneComponent = Cast<USceneComponent>(ActorComponent);
		if (!SceneComponent) continue;

		// Get info structure
		const auto& Info = SpawnObjects.Find(ActorComponent->GetFName());
		if (!Info) continue;

		// Get hard spawn class reference
		const auto& SpawnClass = Info->SpawnClass.LoadSynchronous();
		if (!SpawnClass) continue;

		// Spawn actor
		AActor* SpawnedActor = GetWorld()->SpawnActor(SpawnClass, &SceneComponent->GetComponentTransform());
		if (!SpawnedActor) continue;

		// Get all static mesh components
		TArray<UActorComponent*> Components;
		SpawnedActor->GetComponents(USceneComponent::StaticClass(), Components);
		for (const auto& Component : Components)
		{
			Cast<USceneComponent>(Component)->SetMobility(EComponentMobility::Movable);
		}
		
		SpawnedActor->AttachToComponent(SceneComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

		// If static mesh actor
		if (SpawnedActor->IsA(AStaticMeshActor::StaticClass()))
		{
			const auto& MeshComp = Cast<UStaticMeshComponent>(SpawnedActor->GetRootComponent());
			if (!MeshComp)
			{
				SpawnedActor->Destroy();
				continue;
			}

			UStaticMesh* Mesh = Info->ClassMesh.LoadSynchronous();
			if (!Mesh)
			{
				SpawnedActor->Destroy();
				continue;
			}
			MeshComp->SetStaticMesh(Mesh);
		}
	}
}
