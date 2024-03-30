// Sky Faller. All rights reserved.


#include "Prefab/EXPrefabCreator.h"

#include "BPFL/EXEditorFunctions.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

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
		UE_LOG(LogTemp, Warning, TEXT("Prefab not found."));
		return;
	}

	// Remove self from found actors.
	Actors.Remove(this);

	// Validate parent class.
	if (!Prefab->GetClass()->IsChildOf(ParentClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("Parent class not valid."));
		return;
	}

	// Create actor from parent class to transfer platform params.
	AEXPrefab* ParentActor = Cast<AEXPrefab>(GetWorld()->SpawnActor(ParentClass));
	if (!ParentActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Parent actor not spawned."));
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
		ParentActor->AttachToComponent(ParentActor->GetRootComponent(),
		                               FAttachmentTransformRules::KeepRelativeTransform);
		NewSceneComponent->RegisterComponent();
		ParentActor->AddInstanceComponent(NewSceneComponent);

		if (Actor->IsA(AStaticMeshActor::StaticClass()))
		{
			const auto& MeshComp = Cast<UStaticMeshComponent>(Actor->GetRootComponent());
			if (!(MeshComp && MeshComp->GetStaticMesh())) continue;

			ParentActor->SpawnObjects.Add(Actor->GetFName(),
			                              FPrefabInfo(Actor->GetClass(), MeshComp->GetStaticMesh()));
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
		UE_LOG(LogTemp, Warning, TEXT("Prefab not created."));
		return;
	}

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%s"), *PrefabBlueprint->GetPathName()));
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%s"), *Prefab->GetClass()->GetPathName()));

	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("New prefab created."));
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
