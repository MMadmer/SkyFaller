// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFPlatform.generated.h"

class UStaticMeshComponent;
class ABaseCharacter;
class ASFTarget;
class USFTrapComponent;

USTRUCT(BlueprintType)
struct FAssets
{
	GENERATED_USTRUCT_BODY()

	FAssets()
	{
		Platform = nullptr;
		Skin = nullptr;
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMesh* Platform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMesh* Skin;
};

UCLASS()
class SKYFALLER_API ASFPlatform : public AActor
{
	GENERATED_BODY()

public:
	ASFPlatform();
	virtual void Tick(float DeltaTime) override;

	UStaticMeshComponent* GetMesh() const { return PlatformMesh; };
	int32 GetSelfID() const { return SelfID; };

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Assets")
	TArray<FAssets> Assets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float Threshold = 750.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float ThresholdOffset = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float Speed = 75.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0", ClampMax = "1"))
	float MinSpeed = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0"))
	float Frequency = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0"))
	float Amplitude = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn", meta = (ClampMin = "0", ClampMax = "180"))
	float SpawnAngle = 40.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn")
	float SpawnY = 5000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn")
	float SpawnHeight = -2000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn")
	float SpawnSpeed = 1500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn")
	float DespawnDist = 7000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn")
	float DespawnSpeed = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn")
	float DespawnRotationSpeed = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn", meta = (ClampMin = "0"))
	float MaxDist = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn", meta = (ClampMin = "0"))
	float MinDist = 350.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn")
	int32 HubIndentID = 20;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USFTrapComponent* TrapComponent;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* SkinMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TSubclassOf<ASFPlatform> PlatformClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TSubclassOf<ASFTarget> TargetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Points system")
	float RewardPoints = 5.0f;

	virtual void BeginPlay() override;

	bool bTouched = false;
	float Offset = 0.0f;
	bool Spawned = true;
	float ParentZ = 0.0f;
	float LocalTime = 0.0f;
	bool bDespawned = false;
	TArray<int32> AssetsIndexes;
	const float MESH_RADIUS = 742.0f; // Platform "bounds"(not real)
	float GlobalRotation = 0.0f;
	float ZeroY = 0.0f;
	int32 SelfID = 0;
	bool bIsHub = false;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	           FVector NormalImpulse, const FHitResult& Hit);

	void SetTemplate();
	void SpawnNext(UWorld* World, ABaseCharacter* Player);
	static void ScoringPoints(const ABaseCharacter* Player, float Points);
	void Spawner(float DeltaTime);
	void Despawner(float DeltaTime);
	void Mover(float DeltaTime);
	void SpawnTarget(UWorld* World, ABaseCharacter* Player, ASFPlatform* NewTarget) const;

	void ListenerConnecting();
};
