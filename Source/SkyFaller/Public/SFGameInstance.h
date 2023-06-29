// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SFGameInstance.generated.h"

class USoundCue;
class UAudioComponent;

UCLASS()
class SKYFALLER_API USFGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void PlayLevelAmbient(UAudioComponent* WorldAmbient, TArray<USoundCue*> CurrentLevelAmbient);

private:
	TArray<USoundCue*> LevelAmbient;
	UAudioComponent* AudioComponent;
	TArray<int32> Indexes;

	UFUNCTION()
	void CycleAmbient();
};
