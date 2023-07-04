// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SFCoreTypes.h"
#include "SFWeaponComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartFire);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopFire);

class ASFBaseWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SKYFALLER_API USFWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USFWeaponComponent();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	ASFBaseWeapon* GetCurrentWeapon() const;

	UPROPERTY(BlueprintAssignable)
	FOnStartFire OnStartFire;

	UPROPERTY(BlueprintAssignable)
	FOnStopFire OnStopFire;

	void StartFire();
	void StopFire();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ASFBaseWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponEquipSocketName = "WeaponSocket";

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	ASFBaseWeapon* CurrentWeapon = nullptr;

	void SpawnWeapon();
	void AttachWeaponToSocket(ASFBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);
	bool CanFire();
};
