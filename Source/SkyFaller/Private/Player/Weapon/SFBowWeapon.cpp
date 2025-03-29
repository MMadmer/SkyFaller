// Sky Faller. All rights reserved.


#include "Player/Weapon/SFBowWeapon.h"

void ASFBowWeapon::SetCharge(const float NewCharge)
{
	Charge = FMath::Clamp(NewCharge, 0.0f, ChargeTime);
	OnChargeChanged.Broadcast(GetChargeNorm());
}

void ASFBowWeapon::StartCharging()
{
	if (IsCharged()) return;

	const UWorld* World = GetWorld();
	if (World)
	{
		GetWorld()->GetTimerManager().SetTimer(ChargeTimer, this, &ASFBowWeapon::Charging,
		                                       ChargeTime / static_cast<float>(ChargeFramerate), true);
	}
}

void ASFBowWeapon::StopCharging()
{
	const UWorld* World = GetWorld();
	if (World) GetWorld()->GetTimerManager().ClearTimer(ChargeTimer);
}

void ASFBowWeapon::Charging()
{
	if (IsCharged())
	{
		const UWorld* World = GetWorld();
		if (!World) return;

		World->GetTimerManager().ClearTimer(ChargeTimer);
	}
	else
	{
		SetCharge(Charge + ChargeTime / static_cast<float>(ChargeFramerate));
		OnChargeChanged.Broadcast(GetChargeNorm());
	}
}
