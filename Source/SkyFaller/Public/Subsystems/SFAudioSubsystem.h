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
	virtual void Deinitialize() override;
	
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

	float BaseAudioVolume;
	TSet<TSoftObjectPtr<USoundBase>> LevelAmbient;
	TBitArray<> Indexes;

	UFUNCTION()
	void CycleAmbient();
};
