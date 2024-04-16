// Sky Faller. All rights reserved.


#include "Components/SFHealthComponent.h"
#include "SFCoreTypes.h"
#include "Damage/SFPureDamage.h"
#include "GameFramework/DamageType.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, All, All)

USFHealthComponent::USFHealthComponent(): bImmortal(false)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USFHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	check(MaxHealth > 0);

	SetHealth(MaxHealth);
	SetBarriers(MaxBarriers);

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &USFHealthComponent::OnTakeAnyDamage);
}

void USFHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                         AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || IsDead() || !GetWorld()) return;

	// Check if pure damage
	if (Cast<USFPureDamage>(DamageType))
	{
		SetHealth(Health - Damage);
		return;
	}

	if (bImmortal) return;

	if (Barriers > 0)
	{
		SetBarriers(Barriers - 1);
		return;
	}

	SetHealth(Health - Damage);
}

void USFHealthComponent::SetHealth(const float NewHealth)
{
	if (FMath::IsNearlyEqual(NewHealth, Health)) return;

	const float NextHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
	const float HealthDelta = NextHealth - Health;

	Health = NextHealth;
	OnHealthChanged.Broadcast(Health, HealthDelta);

	if (IsDead()) OnDeath.Broadcast();
}

void USFHealthComponent::SetBarriers(const uint8 NewBarriers)
{
	const uint8 NextBarriers = FMath::Clamp(NewBarriers, static_cast<uint8>(0), MaxBarriers);
	const uint8 BarriersDelta = NextBarriers - Barriers;

	Barriers = NextBarriers;
	OnBarriersChanged.Broadcast(Barriers, BarriersDelta);
}
