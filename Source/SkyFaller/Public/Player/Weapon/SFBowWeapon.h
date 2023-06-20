// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Player/Weapon/SFBaseWeapon.h"
#include "SFBowWeapon.generated.h"

class ASFArrow;
class ABaseCharacter;

UCLASS()
class SKYFALLER_API ASFBowWeapon : public ASFBaseWeapon
{
	GENERATED_BODY()

public:
	virtual void StartFire() override;
	virtual void StopFire() override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<ASFArrow> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* AimAnimMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* PlayerAimAnimMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> PlayerAimBP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	float ChargeTime = 3.0f;

	virtual void MakeShot() override;

private:
	FName BoneBowstring = "DummyMiddle";
	bool bCharged = false;
	FTimerHandle ChargeTimer;

	bool CanFire() const;
	void Charging();
	void BowstringOffset(float Offset);
};
