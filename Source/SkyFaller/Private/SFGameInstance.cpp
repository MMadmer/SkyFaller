// Sky Faller. All rights reserved.


#include "SFGameInstance.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY_STATIC(LogGameInstance, All, All)

void USFGameInstance::PlayLevelAmbient(UAudioComponent* WorldAmbient, TArray<USoundCue*> CurrentLevelAmbient)
{
	AudioComponent = WorldAmbient;
	AudioComponent->OnAudioFinished.AddDynamic(this, &USFGameInstance::CycleAmbient);
	if (LevelAmbient.Num() > 0) LevelAmbient.Empty();
	LevelAmbient.Append(CurrentLevelAmbient);
	CycleAmbient();
}

void USFGameInstance::CycleAmbient()
{
	if (!GetWorld()) return;

	int32 CurrentIndex;
	USoundCue* Song;

	if (Indexes.Num() >= LevelAmbient.Num()) Indexes.Empty();

	if (!LevelAmbient.IsValidIndex(0)) return;

	do
	{
		CurrentIndex = FMath::RandRange(0, LevelAmbient.Num() - 1);

	} while (Indexes.Contains(CurrentIndex));
	Indexes.Add(CurrentIndex);

	Song = LevelAmbient[CurrentIndex];
	if (!Song) return;

	AudioComponent->SetSound(Song);
	AudioComponent->Play();
	UE_LOG(LogGameInstance, Display, TEXT("Ok %s"), *Song->GetName())
}
