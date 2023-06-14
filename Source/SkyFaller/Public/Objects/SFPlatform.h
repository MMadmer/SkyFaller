// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFPlatform.generated.h"

class UStaticMeshComponent;

UCLASS()
class SKYFALLER_API ASFPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	ASFPlatform();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float Threshold = 750.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float ThresholdOffset = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float Speed = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float SpawnHeight = -2000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float SpawnSpeed = 2000.0f;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TSubclassOf<ASFPlatform> PlatformClass;

	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	bool bTouched = false;
	float Offset = 0.0f;
	bool Spawned = true;
	float ParentZ = 0.0f;

	UFUNCTION()
	void SpawnNext(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void Spawner(float DeltaTime);
	void Mover(float DeltaTime);

	void FogConnecting();

};
