// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/BaseCharacter.h"
#include "SFBaseWeapon.generated.h"

class USkeletalMeshComponent;

UCLASS()
class SKYFALLER_API ASFBaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	ASFBaseWeapon();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StartFire();
	virtual void StartFire_Implementation() { ; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StopFire();
	virtual void StopFire_Implementation() { ; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnWeaponSet();
	virtual void OnWeaponSet_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnWeaponDrop();
	virtual void OnWeaponDrop_Implementation();

	UFUNCTION(BlueprintPure)
	FName GetEquipSocketName() const { return WeaponEquipSocketName; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> PlayerAnimBP;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponEquipSocketName = "WeaponSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FName MuzzleSocketName = "MuzzleSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (ClapMin = "0.0"))
	float TraceMaxDistance = 1500.0f;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool MakeShot();
	virtual bool MakeShot_Implementation() { return true; }

	UFUNCTION(BlueprintCallable)
	virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd);

	UFUNCTION(BlueprintCallable)
	void MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd);

	APlayerController* GetPlayerController() const;
	bool GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const;

	FVector GetMuzzleWorldLocation() const { return WeaponMesh->GetSocketLocation(MuzzleSocketName); }
	FRotator GetMuzzleWorldRotation() const { return WeaponMesh->GetSocketRotation(MuzzleSocketName); }

	UClass* CachedPlayerBP;
};
