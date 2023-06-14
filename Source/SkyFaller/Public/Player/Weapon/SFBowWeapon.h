// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Player/Weapon/SFBaseWeapon.h"
#include "SFBowWeapon.generated.h"

class ASFArrow;

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

	virtual void MakeShot() override;

private:
	FName BoneBowstring = "DummyMiddle";
	bool bCharged = false;

	bool CanFire() const;
	void Charging();
	void BowstringOffset(float Offset);
};
