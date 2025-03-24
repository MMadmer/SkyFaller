// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Player/Weapon/SFBaseWeapon.h"
#include "SFBowWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChargeChanged, float, ChargeNorm);

class ASFArrow;

UCLASS()
class SKYFALLER_API ASFBowWeapon : public ASFBaseWeapon
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnChargeChanged OnChargeChanged;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	float GetCharge() const { return Charge / ChargeTime; };

	virtual void StartFire_Implementation() override;
	virtual void StopFire_Implementation() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<ASFArrow> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* PlayerAimAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation", meta = (ClampMin = "0"))
	float ChargeTime = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation", meta = (ClampMin = "0.01"))
	float ChargeSpeed = 0.17f;

	virtual bool MakeShot_Implementation() override;

private:
	uint8 bCharged : 1;
	FTimerHandle ChargeTimer;
	float Charge = 0.0f;

	bool CanFire() const { return bCharged; }
	void Charging();
	void SeriesCalc() const;
};
