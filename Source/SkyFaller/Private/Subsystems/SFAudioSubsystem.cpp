// Sky Faller. All rights reserved.


#include "Subsystems/SFAudioSubsystem.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

void USFAudioSubsystem::PlayLevelAmbient(UAudioComponent* WorldAmbient, const TArray<USoundCue*> CurrentLevelAmbient)
{
	AudioComponent = WorldAmbient;
	if (!AudioComponent) return;

	AudioComponent->OnAudioFinished.AddDynamic(this, &USFAudioSubsystem::CycleAmbient);
	if (LevelAmbient.Num() > 0) LevelAmbient.Empty();
	LevelAmbient.Append(CurrentLevelAmbient);
	CycleAmbient();
}

void USFAudioSubsystem::CycleAmbient()
{
	if (!GetWorld()) return;

	int32 CurrentIndex;

	if (Indexes.Num() >= LevelAmbient.Num()) Indexes.Empty();

	if (!LevelAmbient.IsValidIndex(0)) return;

	do
	{
		CurrentIndex = FMath::RandRange(0, LevelAmbient.Num() - 1);
	}
	while (Indexes.Contains(CurrentIndex));
	Indexes.Add(CurrentIndex);

	USoundCue* Song = LevelAmbient[CurrentIndex];
	if (!Song) return;

	AudioComponent->SetSound(Song);
	AudioComponent->Play();
	// UE_LOG(LogGameInstance, Display, TEXT("Ok %s"), *Song->GetName())
}
