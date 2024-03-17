// Sky Faller. All rights reserved.


#include "Components/SFTrapComponent.h"
#include "Objects/SFTrap.h"
#include "Objects/SFPlatform.h"

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
	const auto World = GetWorld();
	if (!World) return;
	const auto Platform = Cast<ASFPlatform>(GetOwner());
	if (!Platform) return;

	for (auto Trap : Traps)
	{
		// Sockets check
		FName Socket = GetRandomSocket(Cast<ASFTrap>(Trap->GetDefaultObject()),
		                               Platform->GetMesh()->GetAllSocketNames());
		if (!Platform->GetMesh()->DoesSocketExist(Socket)) continue;
		// Spawn chance
		if (std::roundf(FMath::RandRange(0.0f, 100.0f) * 100.0f) / 100.0f > Cast<ASFTrap>(Trap->GetDefaultObject())->
			GetSpawnChance())
			continue;

		const auto NewTrap = World->SpawnActor<ASFTrap>(Trap);
		if (!NewTrap) return;

		NewTrap->SetOwner(Platform);
		AttachTrapToSocket(NewTrap, Platform->GetMesh(), Socket);
	}
}

void USFTrapComponent::AttachTrapToSocket(ASFTrap* Trap, USceneComponent* SceneComponent, const FName& SocketName)
{
	if (!Trap || !SceneComponent) return;

	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
	Trap->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
}

FName USFTrapComponent::GetRandomSocket(const ASFTrap* Trap, TArray<FName> Sockets)
{
	const FString TrapSocket = Trap->GetSocketName().ToString();
	TArray<FName> ConfirmedSockets;
	for (const auto& Socket : Sockets)
	{
		if (Socket.ToString().StartsWith(TrapSocket)) ConfirmedSockets.Add(Socket);
	}

	return ConfirmedSockets.Num() > 0 ? ConfirmedSockets[FMath::RandRange(0, ConfirmedSockets.Num() - 1)] : "";
}
