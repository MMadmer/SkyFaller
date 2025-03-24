// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/Weapon/SFBaseWeapon.h"
#include "SFWeaponComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartFire);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopFire);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, ASFBaseWeapon*, CurrentWeapon);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKYFALLER_API USFWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USFWeaponComponent();

	UPROPERTY(BlueprintAssignable)
	FOnStartFire OnStartFire;

	UPROPERTY(BlueprintAssignable)
	FOnStopFire OnStopFire;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponChanged OnWeaponChanged;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetCurrentWeapon(ASFBaseWeapon* Weapon);

	UFUNCTION(BlueprintPure, Category = "Weapon")
	ASFBaseWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ASFBaseWeapon> WeaponClass;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	ASFBaseWeapon* CurrentWeapon = nullptr;

	void SpawnWeapon();
	static void AttachWeaponToSocket(ASFBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);
	bool CanFire() const { return IsValid(GetCurrentWeapon()); }
};
