// Sky Faller. All rights reserved.


#include "Prefab/UTPrefab.h"

#include "BPFL/UTGlobalFunctions.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"

AUTPrefab::AUTPrefab()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	SetRootComponent(StaticMesh);
}

void AUTPrefab::BeginPlay()
{
	Super::BeginPlay();
}

void AUTPrefab::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ClearAttachedObjects();
}

void AUTPrefab::SpawnAllObjects()
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

			UStaticMesh* Mesh = Info->ClassMesh.StaticMesh.LoadSynchronous();
			if (!Mesh)
			{
				SpawnedActor->Destroy();
				continue;
			}

			// Set saved static mesh and materials
			MeshComp->SetStaticMesh(Mesh);
			int32 MatIndex = 0;
			for (const auto& SoftMaterial : Info->ClassMesh.Materials)
			{
				MeshComp->SetMaterial(MatIndex, SoftMaterial.LoadSynchronous());
				MatIndex++;
			}
			MeshComp->SetCollisionProfileName(Info->CollisionPreset);
			MeshComp->UpdateCollisionFromStaticMesh();
		}
	}
}

void AUTPrefab::ClearAttachedObjects() const
{
	UUTGlobalFunctions::DestroyAttachedActorsByClass(this, AActor::StaticClass());
	ClearAllHism();
}

void AUTPrefab::ConvertMeshToHism()
{
	ClearAllHism();

	TMap<FUniqueMesh, TSet<AStaticMeshActor*>> UniqueMeshes;

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
				break;
			}
		}
		if (!IsValidMaterial) continue;

		// Add unique mesh by static mesh and component materials
		TArray<TSoftObjectPtr<UMaterialInterface>> SoftMaterials;
		for (const auto& Material : MeshComp->GetMaterials())
		{
			SoftMaterials.Add(Material);
		}
		FUniqueMesh Mesh{MeshComp->GetStaticMesh(), SoftMaterials};
		UniqueMeshes.FindOrAdd(Mesh).Add(MeshActor);
	}
	Actors.Empty();

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
		NewHism->UpdateCollisionFromStaticMesh();

		// Update HISM params
		NewHism->SetStaticMesh(UniqueMesh.Key.StaticMesh.LoadSynchronous());
		for (int32 MatIndex = 0; MatIndex < UniqueMesh.Key.Materials.Num(); MatIndex++)
		{
			NewHism->SetMaterial(MatIndex, UniqueMesh.Key.Materials[MatIndex].LoadSynchronous());
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

void AUTPrefab::ClearAllHism() const
{
	UUTGlobalFunctions::DestroyComponentsByClass(this, UHierarchicalInstancedStaticMeshComponent::StaticClass());
}
