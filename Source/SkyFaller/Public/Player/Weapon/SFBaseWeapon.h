// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/BaseCharacter.h"
#include "SFBaseWeapon.generated.h"

class USkeletalMeshComponent;
class USoundCue;

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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundCue* ShotSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FName MuzzleSocketName = "MuzzleSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (ClapMin = "0.0"))
	float TraceMaxDistance = 1500.0f;

	TSubclassOf<UAnimInstance> CachedPlayerBP;

	virtual void MakeShot() { ; }
	virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;
	void MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd) const;

	ABaseCharacter* GetPlayer() const { return Cast<ABaseCharacter>(GetOwner()); }
	APlayerController* GetPlayerController() const;
	bool GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const;

	FVector GetMuzzleWorldLocation() const { return WeaponMesh->GetSocketLocation(MuzzleSocketName); }
	FRotator GetMuzzleWorldRotation() const { return WeaponMesh->GetSocketRotation(MuzzleSocketName); }
};
