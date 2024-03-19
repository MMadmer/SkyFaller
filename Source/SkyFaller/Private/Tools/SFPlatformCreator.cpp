// Sky Faller. All rights reserved.


#include "Tools/SFPlatformCreator.h"

#include "BPFL/SFEditorFunctions.h"
#include "BPFL/SFGeneralFunctions.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

ASFPlatformCreator::ASFPlatformCreator(): PlatformTag(TEXT("platform"))
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

void ASFPlatformCreator::ConvertToPlatform()
{
#if WITH_EDITORONLY_DATA
	ClearPlatform();

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	// Platform finding
	AActor* Platform;
	Actors.Remove(this);
	FindAndRemoveActorByTag(Actors, Platform);
	// if (!Platform) return;

	/*const UStaticMesh* PlatformMesh = Cast<UStaticMeshComponent>(
		Platform->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetStaticMesh();*/
	// if (!PlatformMesh) return;

	for (const auto& Actor : Actors)
	{
		if ((Actor->GetActorLocation() - GetActorLocation()).Size() > Handler->GetCollisionShape().GetSphereRadius())
			continue;

		// USFGeneralFunctions::SpawnActorByClassName(GetWorld(), Actor->GetClass()->GetPathName(), FVector(), FRotator());
		USFEditorFunctions::CreateBlueprintFromActorInstance(
			Actor, "/Game/SkyFaller/GameLogic/Blueprints/Objects/MeshVariations/", Actor->GetClass()->GetName());

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%s"), *Actor->GetClass()->GetPathName()));
	}

	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Platform static mesh updated."));
#endif
}

void ASFPlatformCreator::ClearPlatform() const
{
#if WITH_EDITORONLY_DATA
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	// Platform finding
	AActor* Platform;
	FindAndRemoveActorByTag(Actors, Platform);
	if (!Platform) return;

	// Platform static mesh editing
	const UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(
		Platform->GetComponentByClass(UStaticMeshComponent::StaticClass()));

	if (!MeshComp) return;
	MeshComp->GetStaticMesh()->Sockets.Empty();

	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Platform static mesh cleared."));
#endif
}

void ASFPlatformCreator::FindAndRemoveActorByTag(TArray<AActor*>& Actors, AActor*& Platform) const
{
	for (const auto& Actor : Actors)
	{
		if ((Actor->GetActorLocation() - GetActorLocation()).Size() > Handler->GetCollisionShape().GetSphereRadius())
			continue;

		if (Actor->Tags.Find(PlatformTag) >= 0)
		{
			Platform = Actor;
			break;
		}
	}
	Actors.Remove(Platform);
}
