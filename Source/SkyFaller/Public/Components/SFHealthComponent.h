// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SFCoreTypes.h"
#include "SFHealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SKYFALLER_API USFHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USFHealthComponent();

	UPROPERTY(BlueprintAssignable)
	FOnDeathSignature OnDeath;

	UPROPERTY(BlueprintAssignable)
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "0"))
	float MaxHealth = 100.0f;

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsDead() const { return FMath::IsNearlyZero(Health); }

	float GetHealth() const { return Health; };

protected:

	virtual void BeginPlay() override;

private:
	float Health = 0.0f;

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void SetHealth(const float NewHealth);
		
};
