// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Objects/SFTrap.h"
#include "SFSpikesTrap.generated.h"

class UStaticMeshComponent;

UCLASS()
class SKYFALLER_API ASFSpikesTrap : public ASFTrap
{
	GENERATED_BODY()

public:
	ASFSpikesTrap();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	float TimerTick = 0.3f;

	virtual void BeginPlay() override;

private:
	FTimerHandle DamageTimerHandle;

	void DealingDamage();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
