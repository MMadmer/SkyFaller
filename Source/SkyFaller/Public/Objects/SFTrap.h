// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <cmath>
#include "SFTrap.generated.h"

class UStaticMeshComponent;
class USoundCue;

UCLASS()
class SKYFALLER_API ASFTrap : public AActor
{
	GENERATED_BODY()

public:
	ASFTrap();

	FName GetSocketName() const { return SocketName; };
	float GetSpawnChance() const { return std::roundf(SpawnChance * 100.0f) / 100.0f; };

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* TrapMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data", meta = (ClampMin = "0", ClampMax = "100"))
	float SpawnChance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	FName SocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds")
	USoundCue* TrapSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds", meta = (ClampMin = "0"))
	float TrapSoundRadius = 500.0f;

	virtual void BeginPlay() override;
	void PlayTrapSound();
};
