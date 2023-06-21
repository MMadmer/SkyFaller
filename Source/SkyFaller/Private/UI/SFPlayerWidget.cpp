// Sky Faller. All rights reserved.


#include "UI/SFPlayerWidget.h"
#include "Components/SFProgressComponent.h"
#include "Components/SFWeaponComponent.h"
#include "Player/Weapon/SFBowWeapon.h"

int32 USFPlayerWidget::GetPlayerScore() const
{
	const auto Player = GetOwningPlayerPawn();
	if (!Player) return 0;

	const auto ProgressComponent = Cast<USFProgressComponent>(Player->GetComponentByClass(USFProgressComponent::StaticClass()));
	if (!ProgressComponent) return 0;

	return ProgressComponent->GetScore();
}

float USFPlayerWidget::GetWeaponCharge() const
{
	const auto Player = GetOwningPlayerPawn();
	if (!Player) return 0.0f;

	const auto WeaponComponent = Cast<USFWeaponComponent>(Player->GetComponentByClass(USFWeaponComponent::StaticClass()));
	if (!WeaponComponent) return 0.0f;

	const auto Weapon = Cast<ASFBowWeapon>(WeaponComponent->GetCurrentWeapon());
	if (!Weapon) return 0.0f;

	return Weapon->GetCharge();
}
