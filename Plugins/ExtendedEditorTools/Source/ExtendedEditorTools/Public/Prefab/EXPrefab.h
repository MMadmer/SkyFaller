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

	/** Mesh to replace if spawn class is static mesh actor only. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Prefab Info")
	FName CollisionPreset;

	FPrefabInfo()
	{
		SpawnClass = nullptr;
		ClassMesh = nullptr;
		CollisionPreset = TEXT("");
	}

	explicit FPrefabInfo(const TSoftClassPtr<AActor>& NewSpawnClass) : FPrefabInfo()
	{
		SpawnClass = NewSpawnClass;
	}

	FPrefabInfo(const TSoftClassPtr<AActor>& NewSpawnClass,
	            const TSoftObjectPtr<UStaticMesh>& NewClassMesh,
	            const FName& NewCollisionPreset) : FPrefabInfo(NewSpawnClass)
	{
		ClassMesh = NewClassMesh;
		CollisionPreset = NewCollisionPreset;
	}
};

UCLASS()
class EXTENDEDEDITORTOOLS_API AEXPrefab : public AActor
{
	GENERATED_BODY()

public:
	/** Base static mesh. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn Info")
	UStaticMeshComponent* StaticMesh;

	/** Objects to spawn. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn Info")
	TMap<FName, FPrefabInfo> SpawnObjects;

	AEXPrefab();

protected:
	UPROPERTY(EditAnywhere, Category="Spawn Info")
	uint8 MinMeshesToHism = 5;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Spawn all objects from SpawnObjects map. */
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Spawn Info", meta=(Keywords="Spawn All Objects"))
	void SpawnAllObjects();

	/** Destroy attached actors(not recursive) and HISM components. */
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Spawn Info", meta=(Keywords="Clear Spawned Objects"))
	void ClearAttachedObjects();

	/** Convert all attached static mesh actors to HISM components. */
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Spawn Info", meta=(Keywords="Convert Static Mesh To HISM"))
	void ConvertMeshToHism();

	void ClearAllHism() const;
};
