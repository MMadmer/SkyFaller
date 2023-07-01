// Sky Faller. All rights reserved.


#include "UI/SFPlayerWidget.h"
#include "Components/SFProgressComponent.h"
#include "Components/SFWeaponComponent.h"
#include "Player/Weapon/SFBowWeapon.h"
#include "Components/SFProgressComponent.h"
#include "Components/SFHealthComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerWidget, All, All)

bool USFPlayerWidget::Initialize()
{
	const auto ProgressComponent = GetProgressComponent();
	if (ProgressComponent)
	{
		ProgressComponent->OnScoreChanged.AddUObject(this, &USFPlayerWidget::OnScoreChanged);
	}

	return Super::Initialize();
}

int32 USFPlayerWidget::GetPlayerScore() const
{
	const auto ProgressComponent = GetProgressComponent();
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

void USFPlayerWidget::OnScoreChanged(int32 Value)
{
	// Current player score
	PlayerScore = GetPlayerScore();

	// Updated Score
	CachedScore = Value;

	if (!GetWorld()) return;
	GetWorld()->GetTimerManager().SetTimer(CachedScoreTimer, this, &USFPlayerWidget::OnScoreTimerEnd, CachedScoreTime, false);
}

USFProgressComponent* USFPlayerWidget::GetProgressComponent() const
{
	const auto Player = GetOwningPlayerPawn();
	if (!Player) return nullptr;

	return Cast<USFProgressComponent>(Player->GetComponentByClass(USFProgressComponent::StaticClass()));
}

void USFPlayerWidget::OnScoreTimerEnd()
{
	// UE_LOG(LogPlayerWidget, Display, TEXT("Entered"));
	if (!GetWorld()) return;

	CachedScore = 0;
}

float USFPlayerWidget::GetPlayerHealth() const
{
	const auto Player = GetOwningPlayerPawn();
	if (!Player) return 0.0f;

	const auto HealthComponent = Cast<USFHealthComponent>(Player->GetComponentByClass(USFHealthComponent::StaticClass()));
	if (!HealthComponent) return 0.0f;

	return HealthComponent->GetHealth() / 100.0f;
}
