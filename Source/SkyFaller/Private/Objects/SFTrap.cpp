// Sky Faller. All rights reserved.


#include "Objects/SFTrap.h"
#include "Components/StaticMeshComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

ASFTrap::ASFTrap()
{
	PrimaryActorTick.bCanEverTick = false;

	TrapMesh = CreateDefaultSubobject<UStaticMeshComponent>("TrapMesh");
	SetRootComponent(TrapMesh);

	TrapSound = nullptr;
}

void ASFTrap::BeginPlay()
{
	Super::BeginPlay();
}

void ASFTrap::PlayTrapSound()
{
	UAudioComponent* AudioComponent = NewObject<UAudioComponent>(this);
	USoundAttenuation* AttenuationSettings = NewObject<USoundAttenuation>(this);
	if (AudioComponent && TrapSound && AttenuationSettings)
	{
		// Set sound radius
		AttenuationSettings->Attenuation.bAttenuate = true;
		AttenuationSettings->Attenuation.AttenuationShape = EAttenuationShape::Sphere;
		AttenuationSettings->Attenuation.AttenuationShapeExtents = FVector(TrapSoundRadius);

		// Play sound
		AudioComponent->SetSound(TrapSound);
		AudioComponent->bAllowSpatialization = true;
		AudioComponent->bAutoDestroy = true;
		AudioComponent->AttenuationSettings = AttenuationSettings;
		AudioComponent->SetWorldLocation(GetActorLocation());
		AudioComponent->Play();
	}
}
