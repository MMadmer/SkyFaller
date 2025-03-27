// Sky Faller. All rights reserved.


#include "Prefab/EXPrefabCreator.h"

#include "BPFL/EXEditorFunctions.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/UTPrefab.h"
#include "Foliage/Public/InstancedFoliageActor.h"

AEXPrefabCreator::AEXPrefabCreator()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bIsEditorOnlyActor = true;

	Handler = CreateDefaultSubobject<USphereComponent>(TEXT("Handler"));
	SetRootComponent(Handler);

	ParentClass = AUTPrefab::StaticClass();
}

void AEXPrefabCreator::BeginPlay()
{
	Super::BeginPlay();

	Destroy();
}

void AEXPrefabCreator::CreatePrefab()
{
#if WITH_EDITOR
	if (!IsValid(Handler)) return;

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	// Platform finding
	const AUTPrefab* Prefab = FindAndRemovePrefab(Actors);
	if (!Prefab)
	{
		UEXEditorFunctions::NotifyWithLog(TEXT("Prefab not found"), Warning, 3.0f);
		return;
	}

	// Remove self from found actors.
	Actors.Remove(this);

	// Validate parent class.
	if (!Prefab->GetClass()->IsChildOf(ParentClass))
	{
		UEXEditorFunctions::NotifyWithLog(TEXT("Parent class not valid"), Warning, 3.0f);
		return;
	}

	// Create actor from parent class to transfer platform params.
	AUTPrefab* ParentActor = Cast<AUTPrefab>(GetWorld()->SpawnActor(ParentClass));
	if (!ParentActor)
	{
		UEXEditorFunctions::NotifyWithLog(TEXT("Parent actor not spawned"), Warning, 3.0f);
		return;
	}
	ParentActor->StaticMesh->SetStaticMesh(Prefab->StaticMesh->GetStaticMesh());

	// Copy actors information to parent actor with scene components creating
	for (const auto& Actor : Actors)
	{
		if ((Actor->GetActorLocation() - GetActorLocation()).Size() > Handler->GetCollisionShape().GetSphereRadius())
		{
			continue;
		}

		// Make new scene component
		USceneComponent* NewSceneComponent = NewObject<USceneComponent>(ParentActor, Actor->GetFName());
		if (!NewSceneComponent) continue;
		NewSceneComponent->SetRelativeTransform(
			Actor->GetActorTransform().GetRelativeTransform(Prefab->GetActorTransform()));
		NewSceneComponent->AttachToComponent(ParentActor->GetRootComponent(),
		                                     FAttachmentTransformRules::KeepRelativeTransform);
		NewSceneComponent->RegisterComponent();
		ParentActor->AddInstanceComponent(NewSceneComponent);

		// Add actor into prefab info
		if (Cast<AStaticMeshActor>(Actor))
		{
			const auto& MeshComp = Cast<UStaticMeshComponent>(Actor->GetRootComponent());
			if (!MeshComp || !MeshComp->GetStaticMesh()) continue;

			// Save unique materials for prefab's static mesh
			TArray<TSoftObjectPtr<UMaterialInterface>> SoftMaterials;
			for (const auto& Material : MeshComp->GetMaterials())
			{
				SoftMaterials.Add(Material);
			}

			// Save prefab info
			ParentActor->SpawnObjects.Add(Actor->GetFName(),
			                              FPrefabInfo(Actor->GetClass(),
			                                          {MeshComp->GetStaticMesh(), SoftMaterials},
			                                          MeshComp->GetCollisionProfileName()));
		}
		else
		{
			ParentActor->SpawnObjects.Add(Actor->GetFName(), FPrefabInfo(Actor->GetClass()));
		}
	}

	// Save foliage
	AInstancedFoliageActor* Foliage = AInstancedFoliageActor::GetInstancedFoliageActorForCurrentLevel(GetWorld());
	if (IsValid(Foliage))
	{
		// Get all foliage instances with all meshes
		for (const auto& Pair : Foliage->GetAllInstancesFoliageType())
		{
			if (!Pair.Value) continue;

			// Get mesh component
			const UHierarchicalInstancedStaticMeshComponent* HISM = Pair.Value->GetComponent();
			if (!IsValid(HISM)) continue;

			// Save unique materials for prefab's static mesh
			TArray<TSoftObjectPtr<UMaterialInterface>> SoftMaterials;
			for (const auto& Material : HISM->GetMaterials())
			{
				SoftMaterials.Add(Material);
			}

			// Find foliage mesh instances inside creator
			const FSphere Sphere{Handler->GetComponentLocation(), Handler->GetCollisionShape().GetSphereRadius()};
			TArray<int32> Instances{};
			Pair.Value->GetInstancesInsideSphere(Sphere, Instances);

			// Make scene components
			for (const auto& Instance : Instances)
			{
				const FName SceneName{FGuid::NewGuid().ToString()};

				// Make new scene component
				USceneComponent* NewSceneComponent = NewObject<USceneComponent>(ParentActor, SceneName);
				if (!NewSceneComponent) continue;

				FTransform InstanceTransform{};
				HISM->GetInstanceTransform(Instance, InstanceTransform, true);
				NewSceneComponent->SetRelativeTransform(
					InstanceTransform.GetRelativeTransform(Prefab->GetActorTransform()));

				NewSceneComponent->AttachToComponent(ParentActor->GetRootComponent(),
				                                     FAttachmentTransformRules::KeepRelativeTransform);
				NewSceneComponent->RegisterComponent();
				ParentActor->AddInstanceComponent(NewSceneComponent);

				ParentActor->SpawnObjects.Add(SceneName,
				                              FPrefabInfo(AStaticMeshActor::StaticClass(),
				                                          {HISM->GetStaticMesh(), SoftMaterials},
				                                          HISM->GetCollisionProfileName()));
			}
		}
	}

	// Create blueprint asset
	const UBlueprint* PrefabBlueprint = UEXEditorFunctions::CreateBlueprintFromActorInstance(
		ParentActor, FString::Printf(TEXT("/Game/%s"), *NewPrefabPath),
		FString::Printf(TEXT("%s"), *NewPrefabBaseName));

	// Parent actor now needn't
	ParentActor->Destroy();

	if (!PrefabBlueprint)
	{
		UEXEditorFunctions::NotifyWithLog(TEXT("Prefab not created"), Warning, 3.0f);
		return;
	}

	UEXEditorFunctions::NotifyWithLog(FString::Printf(TEXT("New prefab created: %s"), *PrefabBlueprint->GetPathName()),
	                                  Display, 3.0f);
#endif
}

void AEXPrefabCreator::ClearPrefab()
{
#if WITH_EDITOR
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	// Platform finding
	const AUTPrefab* Prefab = FindAndRemovePrefab(Actors);
	if (!Prefab)
	{
		UEXEditorFunctions::NotifyWithLog(TEXT("Prefab not found"), Warning, 3.0f);
		return;
	}

	// Remove self from found actors.
	Actors.Remove(this);

	for (const auto& Actor : Actors)
	{
		if ((Actor->GetActorLocation() - GetActorLocation()).Size() > Handler->GetCollisionShape().GetSphereRadius())
			continue;

		Actor->Destroy();
	}

	// Remove foliage
	AInstancedFoliageActor* Foliage = AInstancedFoliageActor::GetInstancedFoliageActorForCurrentLevel(GetWorld());
	if (IsValid(Foliage))
	{
		for (const auto& Pair : Foliage->GetAllInstancesFoliageType())
		{
			if (!Pair.Value) continue;

			FFoliageInfo* Info = Foliage->FindInfo(Pair.Key);
			if (!Info) continue;

			const FSphere Sphere{Handler->GetComponentLocation(), Handler->GetCollisionShape().GetSphereRadius()};
			TArray<int32> Instances;
			Pair.Value->GetInstancesInsideSphere(Sphere, Instances);

			Info->RemoveInstances(Foliage, Instances, true);
		}
	}

#endif
}

AUTPrefab* AEXPrefabCreator::FindAndRemovePrefab(TArray<AActor*>& Actors) const
{
	AUTPrefab* Prefab = nullptr;

	for (const auto& Actor : Actors)
	{
		if ((Actor->GetActorLocation() - GetActorLocation()).Size() > Handler->GetCollisionShape().GetSphereRadius())
			continue;

		if (Cast<AUTPrefab>(Actor))
		{
			Prefab = Cast<AUTPrefab>(Actor);
			break;
		}
	}
	Actors.Remove(Prefab);

	return Prefab;
}
