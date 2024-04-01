// Sky Faller. All rights reserved.


#include "Prefab/EXPrefabCreator.h"

#include "BPFL/EXEditorFunctions.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

AEXPrefabCreator::AEXPrefabCreator()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Handler = CreateDefaultSubobject<USphereComponent>(TEXT("Handler"));

	ParentClass = AEXPrefab::StaticClass();
}

void AEXPrefabCreator::BeginPlay()
{
	Super::BeginPlay();

	Destroy();
}

void AEXPrefabCreator::CreatePrefab()
{
#if WITH_EDITORONLY_DATA
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	// Platform finding
	const AEXPrefab* Prefab = FindAndRemovePrefab(Actors);
	if (!Prefab)
	{
		UEXEditorFunctions::NotifyWithLog(TEXT("Prefab not found"), ELogVerbosity::Warning, 3.0f);
		return;
	}

	// Remove self from found actors.
	Actors.Remove(this);

	// Validate parent class.
	if (!Prefab->GetClass()->IsChildOf(ParentClass))
	{
		UEXEditorFunctions::NotifyWithLog(TEXT("Parent class not valid"), ELogVerbosity::Warning, 3.0f);
		return;
	}

	// Create actor from parent class to transfer platform params.
	AEXPrefab* ParentActor = Cast<AEXPrefab>(GetWorld()->SpawnActor(ParentClass));
	if (!ParentActor)
	{
		UEXEditorFunctions::NotifyWithLog(TEXT("Parent actor not spawned"), ELogVerbosity::Warning, 3.0f);
		return;
	}
	ParentActor->StaticMesh->SetStaticMesh(Prefab->StaticMesh->GetStaticMesh());

	// Copy actors information to parent actor with scene components creating
	for (const auto& Actor : Actors)
	{
		if ((Actor->GetActorLocation() - GetActorLocation()).Size() > Handler->GetCollisionShape().GetSphereRadius())
			continue;

		USceneComponent* NewSceneComponent = NewObject<USceneComponent>(ParentActor, Actor->GetFName());
		if (!NewSceneComponent) continue;
		NewSceneComponent->SetRelativeTransform(
			Actor->GetActorTransform().GetRelativeTransform(Prefab->GetActorTransform()));
		NewSceneComponent->AttachToComponent(ParentActor->GetRootComponent(),
		                                     FAttachmentTransformRules::KeepRelativeTransform);
		NewSceneComponent->RegisterComponent();
		ParentActor->AddInstanceComponent(NewSceneComponent);

		if (Cast<AStaticMeshActor>(Actor))
		{
			const auto& MeshComp = Cast<UStaticMeshComponent>(Actor->GetRootComponent());
			if (!(MeshComp && MeshComp->GetStaticMesh())) continue;

			ParentActor->SpawnObjects.Add(Actor->GetFName(),
			                              FPrefabInfo(Actor->GetClass(), MeshComp->GetStaticMesh(),
			                                          MeshComp->GetCollisionProfileName()));
		}
		else
		{
			ParentActor->SpawnObjects.Add(Actor->GetFName(), FPrefabInfo(Actor->GetClass()));
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
		UEXEditorFunctions::NotifyWithLog(TEXT("Prefab not created"), ELogVerbosity::Warning, 3.0f);
		return;
	}

	UEXEditorFunctions::NotifyWithLog(TEXT("New prefab created"), ELogVerbosity::Display, 3.0f);
	UEXEditorFunctions::NotifyWithLog(FString::Printf(TEXT("%s"), *PrefabBlueprint->GetPathName()),
	                                  ELogVerbosity::Display, 3.0f);
#endif
}

AEXPrefab* AEXPrefabCreator::FindAndRemovePrefab(TArray<AActor*>& Actors) const
{
	AEXPrefab* Prefab = nullptr;

	for (const auto& Actor : Actors)
	{
		if ((Actor->GetActorLocation() - GetActorLocation()).Size() > Handler->GetCollisionShape().GetSphereRadius())
			continue;

		if (Cast<AEXPrefab>(Actor))
		{
			Prefab = Cast<AEXPrefab>(Actor);
			break;
		}
	}
	Actors.Remove(Prefab);

	return Prefab;
}
