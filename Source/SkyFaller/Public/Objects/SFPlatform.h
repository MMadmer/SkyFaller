// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prefab/EXPrefab.h"
#include "SFPlatform.generated.h"

class UStaticMeshComponent;
class ASFTarget;

UCLASS()
class SKYFALLER_API ASFPlatform : public AActor
{
	GENERATED_BODY()

public:
	ASFPlatform();
	virtual void Tick(float DeltaTime) override;

	int32 GetSelfID() const { return SelfID; };

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variation")
	TSet<TSoftClassPtr<AEXPrefab>> Assets;
	TSet<int32> AssetsIndexes;

	UPROPERTY(BlueprintReadOnly, Category="Variation")
	AActor* PlatformVariation;

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
	float SpawnAngle = 30.0f;

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
	uint8 HubIndentID = 20;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<ASFPlatform> PlatformClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Points System")
	TSubclassOf<ASFTarget> TargetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Points System")
	float RewardPoints = 5.0f;

	virtual void BeginPlay() override;

	bool bTouched = false;
	float Offset = 0.0f;
	bool Spawned = true;
	float ParentZ = 0.0f;
	float LocalTime = 0.0f;
	bool bDespawned = false;
	const float PLATFORM_RADIUS = 742.0f; // Platform "bounds"(not real)
	float GlobalRotation = 0.0f;
	float ZeroY = 0.0f;
	int32 SelfID = 0;
	bool bIsHub = false;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	           FVector NormalImpulse, const FHitResult& Hit);

	void SetTemplate();
	void SpawnNext();
	static void ScoringPoints(APlayerState* PlayerState, float Points);
	void Spawner(float DeltaTime);
	void Despawner(float DeltaTime);
	void Mover(float DeltaTime);
	void SpawnTarget() ;

	void ListenerConnecting();
};
