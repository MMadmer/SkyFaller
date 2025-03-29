// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Player/Weapon/SFBaseWeapon.h"
#include "SFBowWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChargeChanged, float, ChargeNorm);

UCLASS()
class SKYFALLER_API ASFBowWeapon : public ASFBaseWeapon
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnChargeChanged OnChargeChanged;

	UFUNCTION(BlueprintPure)
	float GetCharge() const { return Charge; }

	UFUNCTION(BlueprintPure)
	float GetChargeNorm() const
	{
		return FMath::GetMappedRangeValueClamped(FVector2D(0.0f, ChargeTime), FVector2D(0.0f, 1.0f), GetCharge());
	}

	UFUNCTION(BlueprintPure)
	bool IsCharged() const { return GetCharge() >= ChargeTime; }

	UFUNCTION(BlueprintPure)
	bool IsDrawHold() const { return GetChargeNorm() > 0.0f; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float ShotStrength = 5000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (ClampMin = "0.01"))
	float ChargeTime = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (ClampMin = "30", UIMin = "30"))
	uint8 ChargeFramerate = 60;

	UFUNCTION(BlueprintCallable)
	void SetCharge(const float NewCharge);

	UFUNCTION(BlueprintCallable)
	void ResetCharge() { SetCharge(0.0f); }

	UFUNCTION(BlueprintCallable)
	void StartCharging();

	UFUNCTION(BlueprintCallable)
	void StopCharging();

private:
	float Charge = 0.0f;
	FTimerHandle ChargeTimer;

	void Charging();
};
