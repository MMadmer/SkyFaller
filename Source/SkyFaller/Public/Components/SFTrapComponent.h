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

	UFUNCTION(BlueprintCallable, Category="Traps", meta=(Keywords="Spawn Traps"))
	void SpawnTraps();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traps")
	bool bHasLimit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traps", meta=(EditCondition="bHasLimit"))
	bool bWhiteList = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traps", meta=(EditCondition="bHasLimit"))
	TMap<TSubclassOf<ASFTrap>, uint8> Traps;

	virtual void BeginPlay() override;

	static void SpawnTrap(ASFTrap* Trap, uint8* TypeSpawnedCount = nullptr, const uint8* MaxTypeTraps = nullptr);
};
