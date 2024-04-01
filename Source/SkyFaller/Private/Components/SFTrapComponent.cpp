// Sky Faller. All rights reserved.


#include "Components/SFTrapComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogTrapComponent, All, All)

USFTrapComponent::USFTrapComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USFTrapComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USFTrapComponent::SpawnTraps()
{
	TMap<TSubclassOf<ASFTrap>, uint8> SpawnedTraps;
	TArray<AActor*> AttachedActors{};
	GetOwner()->GetAttachedActors(AttachedActors);

	for (const auto& AttachedActor : AttachedActors)
	{
		// Is trap
		const ASFTrap* Trap = Cast<ASFTrap>(AttachedActor);
		if (!Trap) continue;

		if (bHasLimit)
		{
			// Get max traps of type if trap can be spawned
			const uint8* MaxTypeTraps = Traps.Find(Trap->GetClass()->StaticClass());
			if (!MaxTypeTraps)
			{
				AttachedActor->Destroy();
				continue;
			}

			// Get already "spawned" traps by type
			uint8& TypeSpawnedCount = SpawnedTraps.FindOrAdd(Trap->GetClass()->StaticClass());
			if (TypeSpawnedCount >= *MaxTypeTraps)
			{
				AttachedActor->Destroy();
				continue;
			}

			// "Spawn" trap
			if (Trap->GetSpawnChanceNorm() >= FMath::FRandRange(0.0f, 1.0f))
			{
				TypeSpawnedCount++;
				continue;
			}

			AttachedActor->Destroy();
		}
		else
		{
			// "Spawn" trap
			if (Trap->GetSpawnChanceNorm() >= FMath::FRandRange(0.0f, 1.0f))
			{
				continue;
			}

			AttachedActor->Destroy();
		}
	}
}
