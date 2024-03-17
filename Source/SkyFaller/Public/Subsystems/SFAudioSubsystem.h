// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundCue.h"
#include "Subsystems/WorldSubsystem.h"
#include "SFAudioSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SKYFALLER_API USFAudioSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void PlayLevelAmbient(UAudioComponent* WorldAmbient, const TArray<USoundCue*> CurrentLevelAmbient);

private:
	UPROPERTY()
	UAudioComponent* AudioComponent;

	UPROPERTY()
	TArray<USoundCue*> LevelAmbient;

	TArray<int32> Indexes;

	UFUNCTION()
	void CycleAmbient();
};
