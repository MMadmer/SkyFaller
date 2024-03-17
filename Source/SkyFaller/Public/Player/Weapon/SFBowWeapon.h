// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Player/Weapon/SFBaseWeapon.h"
#include "SFBowWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChargeChanged, float, ChargeNorm);

class ASFArrow;
class ABaseCharacter;

UCLASS()
class SKYFALLER_API ASFBowWeapon : public ASFBaseWeapon
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnChargeChanged OnChargeChanged;
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	float GetCharge() const { return Charge / ChargeTime; };

	virtual void StartFire() override;
	virtual void StopFire() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<ASFArrow> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* PlayerAimAnimMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> PlayerAimBP;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> AnimationInst;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation", meta = (ClampMin = "0"))
	float ChargeTime = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation", meta = (ClampMin = "0.01"))
	float ChargeSpeed = 0.1f;

	virtual void MakeShot() override;

	virtual void BeginPlay() override;

private:
	FName BoneBowstring = "DummyMiddle";
	bool bCharged = false;
	FTimerHandle ChargeTimer;
	float Charge = 0.0f;

	bool CanFire() const;
	void Charging();
	void BowstringOffset(float Offset) const;
	void SeriesCalc() const;
};
