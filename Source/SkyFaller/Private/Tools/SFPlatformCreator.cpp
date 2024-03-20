// Sky Faller. All rights reserved.


#include "Tools/SFPlatformCreator.h"

#include "BPFL/SFEditorFunctions.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

ASFPlatformCreator::ASFPlatformCreator()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Handler = CreateDefaultSubobject<USphereComponent>(TEXT("Handler"));
}

void ASFPlatformCreator::BeginPlay()
{
	Super::BeginPlay();

	Destroy();
}

void ASFPlatformCreator::CreatePlatform()
{
#if WITH_EDITORONLY_DATA
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	// Platform finding
	const ASFPlatformMesh* Platform = FindAndRemovePlatform(Actors);
	if (!Platform)
	{
		UE_LOG(LogTemp, Warning, TEXT("Platform not found."));
		return;
	}

	// Remove self from found actors.
	Actors.Remove(this);

	// Validate parent class.
	if (!Platform->GetClass()->IsChildOf(ParentClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("Parent class not valid."));
		return;
	}

	// Create actor from parent class to transfer platform params.
	ASFPlatformMesh* ParentActor = Cast<ASFPlatformMesh>(GetWorld()->SpawnActor(ParentClass));
	if (!ParentActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Parent actor not spawned."));
		return;
	}
	ParentActor->StaticMesh->SetStaticMesh(Platform->StaticMesh->GetStaticMesh());

	// Copy actors information to parent actor with scene components creating
	for (const auto& Actor : Actors)
	{
		if ((Actor->GetActorLocation() - GetActorLocation()).Size() > Handler->GetCollisionShape().GetSphereRadius())
			continue;

		USceneComponent* NewSceneComponent = NewObject<USceneComponent>(ParentActor, Actor->GetFName());
		if (!NewSceneComponent) continue;
		NewSceneComponent->SetRelativeTransform(
			Actor->GetActorTransform().GetRelativeTransform(Platform->GetActorTransform()));
		ParentActor->AttachToComponent(ParentActor->GetRootComponent(),
		                               FAttachmentTransformRules::KeepRelativeTransform);
		NewSceneComponent->RegisterComponent();
		ParentActor->AddInstanceComponent(NewSceneComponent);

		if (Actor->IsA(AStaticMeshActor::StaticClass()))
		{
			const auto& MeshComp = Cast<UStaticMeshComponent>(Actor->GetRootComponent());
			if (!(MeshComp && MeshComp->GetStaticMesh())) continue;

			ParentActor->SpawnObjects.Add(Actor->GetFName(),
			                              FPlatformInfo(Actor->GetClass(), MeshComp->GetStaticMesh()));
		}
		else
		{
			ParentActor->SpawnObjects.Add(Actor->GetFName(), FPlatformInfo(Actor->GetClass()));
		}
	}

	// Create blueprint asset
	const UBlueprint* PlatformBlueprint = USFEditorFunctions::CreateBlueprintFromActorInstance(
		ParentActor, "/Game/SkyFaller/GameLogic/Blueprints/Objects/MeshVariations/",
		FString::Printf(TEXT("%s"), *ParentActor->GetClass()->GetName()));

	// Parent actor now needn't
	ParentActor->Destroy();

	if (!PlatformBlueprint)
	{
		UE_LOG(LogTemp, Warning, TEXT("Platform not created."));
		return;
	}

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%s"), *PlatformBlueprint->GetPathName()));
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%s"), *Platform->GetClass()->GetPathName()));

	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("New platform created."));
#endif
}

void ASFPlatformCreator::ClearPlatform() const
{
#if WITH_EDITORONLY_DATA
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	// Platform finding
	const ASFPlatformMesh* Platform = FindAndRemovePlatform(Actors);
	if (!Platform)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Platform not found."));
		return;
	}

	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Platform was cleared."));
#endif
}

ASFPlatformMesh* ASFPlatformCreator::FindAndRemovePlatform(TArray<AActor*>& Actors) const
{
	ASFPlatformMesh* Platform = nullptr;

	for (const auto& Actor : Actors)
	{
		if ((Actor->GetActorLocation() - GetActorLocation()).Size() > Handler->GetCollisionShape().GetSphereRadius())
			continue;

		if (Cast<ASFPlatformMesh>(Actor))
		{
			Platform = Cast<ASFPlatformMesh>(Actor);
			break;
		}
	}
	Actors.Remove(Platform);

	return Platform;
}
