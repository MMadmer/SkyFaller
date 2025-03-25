// Sky Faller. All rights reserved.


#include "Subsystems/SFAudioSubsystem.h"
#include "Components/AudioComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogAudioSys, All, All);

void USFAudioSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (AudioComponent) AudioComponent->OnAudioFinished.RemoveDynamic(this, &USFAudioSubsystem::CycleAmbient);
}

void USFAudioSubsystem::PlayLevelAmbient(UAudioComponent* WorldAmbient,
                                         const TSet<TSoftObjectPtr<USoundBase>> CurrentLevelAmbient,
                                         const float VolumeNorm)
{
	AudioComponent = WorldAmbient;
	if (!AudioComponent) return;

	// Volume settings
	BaseAudioVolume = AudioComponent->VolumeMultiplier;
	SetAmbientVolume(VolumeNorm);

	AudioComponent->OnAudioFinished.AddUniqueDynamic(this, &USFAudioSubsystem::CycleAmbient);

	if (LevelAmbient.Num() > 0)
	{
		LevelAmbient.Empty();
		Indexes.Init(false, LevelAmbient.Num());
	}

	LevelAmbient = CurrentLevelAmbient;

	CycleAmbient();
}

void USFAudioSubsystem::SetAmbientVolume(const float VolumeNorm) const
{
	if (!AudioComponent) return;

	AudioComponent->SetVolumeMultiplier(FMath::Clamp(BaseAudioVolume * VolumeNorm, 0.001f, 1.0f));
}

void USFAudioSubsystem::CycleAmbient()
{
	if (!AudioComponent) return;

	int32 CurrentIndex;

	if (Indexes.Num() != LevelAmbient.Num() || Indexes.CountSetBits() == Indexes.Num())
		Indexes.Init(
			false, LevelAmbient.Num());

	if (LevelAmbient.Num() == 0)
	{
		UE_LOG(LogAudioSys, Warning, TEXT("Ambient not found"));
		return;
	}

	do
	{
		CurrentIndex = FMath::RandRange(0, LevelAmbient.Num() - 1);
	}
	while (Indexes[CurrentIndex]);
	Indexes[CurrentIndex] = true;

	auto SoundIt = LevelAmbient.CreateIterator();
	for (int32 i = 0; i < CurrentIndex; ++i)
	{
		++SoundIt;
	}

	if (SoundIt->IsNull()) return;

	// Check and use if already loaded
	USoundBase* Sound = SoundIt->Get();
	if (Sound)
	{
		AudioComponent->SetSound(Sound);
		AudioComponent->Play();
	}
	else
	{
		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();

		TWeakObjectPtr<USFAudioSubsystem> WeakThis(this);
		TSoftObjectPtr<USoundBase> SoftSound = *SoundIt;

		StreamableManager.RequestAsyncLoad(SoftSound.ToSoftObjectPath(), [WeakThis, SoftSound]()
		{
			if (!WeakThis.IsValid()) return;

			USoundBase* LoadedSound = SoftSound.Get();
			if (!LoadedSound)
			{
				UE_LOG(LogAudioSys, Warning, TEXT("Failed to async load sound."));
				return;
			}

			const USFAudioSubsystem* Subsystem = WeakThis.Get();
			if (IsValid(Subsystem->AudioComponent))
			{
				Subsystem->AudioComponent->SetSound(LoadedSound);
				Subsystem->AudioComponent->Play();
			}
		});
	}
}
