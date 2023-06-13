// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SFWeaponComponent.generated.h"


class ASFBaseWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SKYFALLER_API USFWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USFWeaponComponent();

	void StartFire();
	void StopFire();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ASFBaseWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponEquipSocketName = "WeaponSocket";

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	ASFBaseWeapon* CurrentWeapon = nullptr;

	void SpawnWeapon();
	bool CanFire();
};
