// Sky Faller. All rights reserved.


#include "Components/SFHealthComponent.h"
#include "SFCoreTypes.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, All, All)

USFHealthComponent::USFHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USFHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	check(MaxHealth > 0);

	SetHealth(MaxHealth);

	AActor* ComponentOwner = GetOwner();
	if (ComponentOwner)
	{
		ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &USFHealthComponent::OnTakeAnyDamage);
	}
}

void USFHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || IsDead() || !GetWorld()) return;

	SetHealth(Health - Damage);

	if (IsDead()) OnDeath.Broadcast();
}

void USFHealthComponent::SetHealth(float NewHealth)
{
	const auto NextHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
	const auto HealthDelta = NextHealth - Health;

	Health = NextHealth;
	OnHealthChanged.Broadcast(Health, HealthDelta);
}
