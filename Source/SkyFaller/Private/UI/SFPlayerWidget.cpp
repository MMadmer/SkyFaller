// Sky Faller. All rights reserved.


#include "UI/SFPlayerWidget.h"
#include "Components/SFWeaponComponent.h"
#include "Player/Weapon/SFBowWeapon.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerWidget, All, All)

float USFPlayerWidget::GetWeaponCharge() const
{
	const auto Player = GetOwningPlayerPawn();
	if (!Player) return 0.0f;

	const auto WeaponComponent = Cast<USFWeaponComponent>(
		Player->GetComponentByClass(USFWeaponComponent::StaticClass()));
	if (!WeaponComponent) return 0.0f;

	const auto Weapon = Cast<ASFBowWeapon>(WeaponComponent->GetCurrentWeapon());
	if (!Weapon) return 0.0f;

	return Weapon->GetCharge();
}

void USFPlayerWidget::OnScoreChanged(const int32 Value)
{
	// Updated Score
	CachedScore = Value;

	if (!GetWorld()) return;
	GetWorld()->GetTimerManager().SetTimer(CachedScoreTimer, this, &USFPlayerWidget::OnScoreTimerEnd, CachedScoreTime,
	                                       false);
}

void USFPlayerWidget::OnScoreTimerEnd()
{
	if (!GetWorld()) return;

	CachedScore = 0;
}
