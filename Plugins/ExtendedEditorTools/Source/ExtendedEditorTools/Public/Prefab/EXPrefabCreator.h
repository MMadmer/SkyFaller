// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Prefab/EXPrefab.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "EXPrefabCreator.generated.h"

UCLASS()
class EXTENDEDEDITORTOOLS_API AEXPrefabCreator : public AActor
{
	GENERATED_BODY()

public:
	AEXPrefabCreator();

protected:
	
	/** Template of generated platforms class. */
	UPROPERTY(EditAnywhere, Category="Editor Tools")
	TSubclassOf<AEXPrefab> ParentClass;

	UPROPERTY(EditAnywhere, Category="Editor Tools")
	FString NewPrefabBaseName = TEXT("Prefab");
	
	/** Path to new prefab asset. Already contains /Game/ */
	UPROPERTY(EditAnywhere, Category="Editor Tools")
	FString NewPrefabPath = TEXT("");

	/** Working radius. */
	UPROPERTY(EditAnywhere, Category="Editor Tools")
	USphereComponent* Handler;

	/** Convert all actors to platform's static mesh sockets(set PlatformTag to target platform static mesh actor). */
	UFUNCTION(CallInEditor, Category="Editor Tools")
	void CreatePrefab();

	virtual void BeginPlay() override;

	AEXPrefab* FindAndRemovePrefab(TArray<AActor*>& Actors) const;
};
