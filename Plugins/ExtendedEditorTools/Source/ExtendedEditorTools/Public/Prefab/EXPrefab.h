// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EXPrefab.generated.h"

USTRUCT(BlueprintType)
struct FPrefabInfo
{
	GENERATED_BODY()

	/** Class that will be spawned and attached to target scene component. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Prefab Info")
	TSoftClassPtr<AActor> SpawnClass;

	/** Mesh to replace if spawn class is static mesh actor only. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Prefab Info")
	TSoftObjectPtr<UStaticMesh> ClassMesh;

	FPrefabInfo()
	{
		SpawnClass = nullptr;
		ClassMesh = nullptr;
	}

	explicit FPrefabInfo(const TSoftClassPtr<AActor>& NewSpawnClass) : FPrefabInfo()
	{
		SpawnClass = NewSpawnClass;
	}

	FPrefabInfo(const TSoftClassPtr<AActor>& NewSpawnClass,
		const TSoftObjectPtr<UStaticMesh>& NewClassMesh) : FPrefabInfo(NewSpawnClass)
	{
		ClassMesh = NewClassMesh;
	}
};

UCLASS()
class EXTENDEDEDITORTOOLS_API AEXPrefab : public AActor
{
	GENERATED_BODY()
	
public:

	/** Base static mesh. */
	UPROPERTY(EditAnywhere, Category="Spawn Info")
	UStaticMeshComponent* StaticMesh;

	/** Objects to spawn. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn Info")
	TMap<FName, FPrefabInfo> SpawnObjects;

	AEXPrefab();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(CallInEditor, Category="Spawn Info")
	void SpawnAllObjects();

	UFUNCTION(CallInEditor, Category="Spawn Info")
	void ClearSpawnedObjects() const;
};
