// Sky Faller. All rights reserved.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFPlatformMesh.generated.h"

USTRUCT(BlueprintType)
struct FPlatformInfo
{
	GENERATED_BODY()

	/** Class that will be spawned and attached to target scene component. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<AActor> SpawnClass;

	/** Mesh to replace if spawn class is static mesh actor only. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> ClassMesh;

	FPlatformInfo()
	{
		SpawnClass = nullptr;
		ClassMesh = nullptr;
	}

	explicit FPlatformInfo(const TSoftClassPtr<AActor>& NewSpawnClass) : FPlatformInfo()
	{
		SpawnClass = NewSpawnClass;
	}

	FPlatformInfo(const TSoftClassPtr<AActor>& NewSpawnClass,
	              const TSoftObjectPtr<UStaticMesh>& NewClassMesh) : FPlatformInfo(NewSpawnClass)
	{
		ClassMesh = NewClassMesh;
	}
};

UCLASS()
class SKYFALLER_API ASFPlatformMesh : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, FPlatformInfo> SpawnObjects;

	ASFPlatformMesh();

protected:
	virtual void BeginPlay() override;

	void SpawnAllObjects();
};
