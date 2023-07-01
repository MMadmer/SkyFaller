// Sky Faller. All rights reserved.


#include "Components/SFTrapComponent.h"
#include "Objects/SFTrap.h"
#include "Objects/SFPlatform.h"
#include <cmath>

DEFINE_LOG_CATEGORY_STATIC(LogTrapComponent, All, All)

USFTrapComponent::USFTrapComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void USFTrapComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnTraps();
}

void USFTrapComponent::SpawnTraps()
{
	const auto World = GetWorld();
	if (!World) return;
	const auto Platform = Cast<ASFPlatform>(GetOwner());
	if (!Platform) return;

	for (auto Trap : Traps)
	{
		// Spawn chance
		if ((std::roundf(FMath::RandRange(0.0f, 100.0f) * 100.0f) / 100.0f) > Cast<ASFTrap>(Trap->GetDefaultObject())->GetSpawnChance()) continue;

		const auto NewTrap = World->SpawnActor<ASFTrap>(Trap);
		if (!NewTrap) return;

		NewTrap->SetOwner(Platform);
		AttachTrapToSocket(NewTrap, Platform->GetMesh(), NewTrap->GetSocketName());
	}
}

void USFTrapComponent::AttachTrapToSocket(ASFTrap* Trap, USceneComponent* SceneComponent, const FName& SocketName)
{
	if (!Trap || !SceneComponent) return;

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
	Trap->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
}
