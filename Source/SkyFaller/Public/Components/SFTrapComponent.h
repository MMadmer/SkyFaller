// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SFTrap.h"
#include "Components/ActorComponent.h"
#include "SFTrapComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKYFALLER_API USFTrapComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USFTrapComponent();

	void SpawnTraps();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Traps")
	TMap<TSubclassOf<ASFTrap>, uint8> Traps;

	virtual void BeginPlay() override;
};
