// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Objects/SFTrap.h"
#include "SFBearTrap.generated.h"

class UStaticMeshComponent;

UCLASS()
class SKYFALLER_API ASFBearTrap : public ASFTrap
{
	GENERATED_BODY()

public:
	ASFBearTrap();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* FirstMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* SecondMesh;

protected:
	virtual void BeginPlay() override;

private:
	bool bActivated = false;
	FTimerHandle AnimHandle;
	float AnimTime = 0.1f;
	float AnimTick = 0.01f;
	float Offset = 86.0f * (AnimTick / AnimTime);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	           FVector NormalImpulse, const FHitResult& Hit);

	void Animation();
};
