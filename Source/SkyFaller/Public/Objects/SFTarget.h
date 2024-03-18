// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFTarget.generated.h"

class UStaticMeshComponent;

UCLASS()
class SKYFALLER_API ASFTarget : public AActor
{
	GENERATED_BODY()

public:
	ASFTarget();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Points system")
	int32 RewardPoints = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Points system")
	int32 SeriesPoints = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tools")
	float LifeSpan = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tools", meta = (ClapMin = "0", ClapMax = "100"))
	float SpeedTransmission = 0.1f;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	           FVector NormalImpulse, const FHitResult& Hit);

	bool bHitted = false;
};
