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
		ASFTrap* Trap = Cast<ASFTrap>(AttachedActor);
		if (!Trap) continue;

		if (bHasLimit)
		{
			if (bWhiteList)
			{
				// Get max traps of type if trap can be spawned
				const uint8* MaxTypeTraps = Traps.Find(Trap->GetClass());
				if (!MaxTypeTraps)
				{
					Trap->Destroy();
					continue;
				}

				SpawnTrap(Trap, &SpawnedTraps.FindOrAdd(Trap->GetClass()->StaticClass()), MaxTypeTraps);
			}
			else
			{
				// Get traps from blacklist
				const uint8* MaxTypeTraps = Traps.Find(Trap->GetClass());
				if (MaxTypeTraps)
				{
					SpawnTrap(Trap, &SpawnedTraps.FindOrAdd(Trap->GetClass()->StaticClass()), MaxTypeTraps);
				}
				else
				{
					SpawnTrap(Trap);
				}
			}
		}
		else
		{
			SpawnTrap(Trap);
		}
	}
}

void USFTrapComponent::SpawnTrap(ASFTrap* Trap, uint8* TypeSpawnedCount, const uint8* MaxTypeTraps)
{
	if (TypeSpawnedCount && MaxTypeTraps)
	{
		if (*TypeSpawnedCount >= *MaxTypeTraps)
		{
			Trap->Destroy();
			return;
		}
	}

	if (Trap->GetSpawnChanceNorm() >= FMath::FRandRange(0.0f, 1.0f))
	{
		if (TypeSpawnedCount) (*TypeSpawnedCount)++;
		return;
	}

	Trap->Destroy();
}
