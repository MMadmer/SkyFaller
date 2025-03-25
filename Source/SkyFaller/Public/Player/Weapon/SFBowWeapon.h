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

	UFUNCTION(BlueprintCallable)
	float GetChargeNorm() const { return Charge / ChargeTime; }

	UFUNCTION(BlueprintPure)
	bool IsCharged() const { return bCharged; }

	UFUNCTION(BlueprintPure)
	bool IsDrawHold() const { return GetChargeNorm() > 0.0f; }

	virtual void StartFire_Implementation() override;
	virtual void StopFire_Implementation() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float ShotStrength = 5000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* PlayerAimAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation", meta = (ClampMin = "0.01"))
	float ChargeTime = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation", meta = (ClampMin = "0.01"))
	float ChargeSpeed = 0.17f;

	UFUNCTION(BlueprintCallable)
	void ResetCharge();

	virtual bool MakeShot_Implementation() override;

private:
	uint8 bCharged : 1;
	FTimerHandle ChargeTimer;
	float Charge = 0.0f;

	void Charging();
	void SeriesCalc() const;
};
