// Sky Faller. All rights reserved.


#include "Prefab/EXPrefab.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "ExtendedEditorTools/EXCoreTypes.h"

AEXPrefab::AEXPrefab()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	SetRootComponent(StaticMesh);
}

void AEXPrefab::BeginPlay()
{
	Super::BeginPlay();
}

void AEXPrefab::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ClearAttachedObjects();
}

void AEXPrefab::SpawnAllObjects()
{
	// Get all scene components
	TArray<USceneComponent*> SceneComponents;
	GetComponents<USceneComponent>(SceneComponents);

	for (const auto& SceneComponent : SceneComponents)
	{
		// Get info structure
		const auto& Info = SpawnObjects.Find(SceneComponent->GetFName());
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
			MeshComp->SetCollisionProfileName(Info->CollisionPreset);
			MeshComp->UpdateCollisionProfile();
		}
	}
}

void AEXPrefab::ClearAttachedObjects()
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	for (const auto& AttachedActor : AttachedActors)
	{
		AttachedActor->Destroy();
	}

	ClearAllHism();
}

void AEXPrefab::ConvertMeshToHism()
{
	ClearAllHism();

	TMap<FUniqueMesh, TArray<AStaticMeshActor*>> UniqueMeshes;

	TArray<AActor*> Actors;
	GetAttachedActors(Actors);

	// Get unique meshes
	for (const auto& Actor : Actors)
	{
		// Check to static mesh actor
		const auto& MeshActor = Cast<AStaticMeshActor>(Actor);
		if (!MeshActor) continue;

		// Check static mesh component and static mesh
		const UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(
			Actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (!(MeshComp && MeshComp->GetStaticMesh())) continue;

		bool IsValidMaterial = true;
		for (const auto& Material : MeshComp->GetMaterials())
		{
			if (!Material->GetBaseMaterial()->bUsedWithInstancedStaticMeshes)
			{
				IsValidMaterial = false;
			}
		}
		if (!IsValidMaterial) continue;

		// Add unique mesh by static mesh and component materials
		FUniqueMesh Mesh{MeshComp->GetStaticMesh(), MeshComp->GetMaterials()};
		UniqueMeshes.FindOrAdd(Mesh).Add(MeshActor);
	}

	// Replace unique meshes with HISM
	int32 Counter = 0;
	for (const auto& UniqueMesh : UniqueMeshes)
	{
		// Check by minimum allowed the same static mesh actors count
		if (UniqueMesh.Value.Num() < MinMeshesToHism) continue;
		
		// Create new HISM
		UHierarchicalInstancedStaticMeshComponent* NewHism = NewObject<UHierarchicalInstancedStaticMeshComponent>(
			this, FName(FString::Printf(TEXT("HISM_%i"), Counter++)));
		if (!NewHism) continue;
		NewHism->RegisterComponent();
		AddInstanceComponent(NewHism);
		NewHism->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		NewHism->UpdateCollisionProfile();

		// Update HISM params
		NewHism->SetStaticMesh(UniqueMesh.Key.StaticMesh);
		for (int32 MatIndex = 0; MatIndex < UniqueMesh.Key.Materials.Num(); MatIndex++)
		{
			NewHism->SetMaterial(MatIndex, UniqueMesh.Key.Materials[MatIndex]);
		}

		// Replace static mesh actors with HISM
		bool bFirstActor = true;
		for (const auto& MeshActor : UniqueMesh.Value)
		{
			const auto& MeshComp = MeshActor->GetStaticMeshComponent();
			if (!MeshComp || !MeshComp->GetStaticMesh() || MeshComp->GetStaticMesh() != UniqueMesh.Key.StaticMesh)
				continue;

			FTransform RelativeTransform = MeshActor->GetActorTransform().GetRelativeTransform(GetActorTransform());
			if (bFirstActor)
			{
				NewHism->SetCollisionProfileName(MeshComp->GetCollisionProfileName());
				bFirstActor = false;
			}
			NewHism->AddInstance(RelativeTransform);
			MeshActor->Destroy();
		}
	}
}

void AEXPrefab::ClearAllHism() const
{
	TArray<UHierarchicalInstancedStaticMeshComponent*> Components;
	GetComponents<UHierarchicalInstancedStaticMeshComponent>(Components);

	for (const auto& Component : Components)
	{
		Component->DestroyComponent();
	}
}
