// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SFAudioSubsystem.generated.h"

/**
 * World audio subsystem
 */
UCLASS()
class SKYFALLER_API USFAudioSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Plays cycled level ambient. Songs will not start repeating until the entire array is played back. */
	UFUNCTION(BlueprintCallable, meta=(Keywords="Play Level Ambient"))
	void PlayLevelAmbient(UAudioComponent* WorldAmbient, const TSet<TSoftObjectPtr<USoundBase>> CurrentLevelAmbient,
	                      const float VolumeNorm = 1.0f);

	/**
	 * Set new ambient volume multiplier.
	 * Volume multiplier = base audio component volume * volume normalized.
	 */
	UFUNCTION(BlueprintCallable, meta=(Keywords="Set Ambient Volume"))
	void SetAmbientVolume(const float VolumeNorm) const;

private:
	UPROPERTY()
	UAudioComponent* AudioComponent;

	UPROPERTY()
	TSet<TSoftObjectPtr<USoundBase>> LevelAmbient;

	float BaseAudioVolume;
	TSet<int32> Indexes;

	UFUNCTION()
	void CycleAmbient();
};
