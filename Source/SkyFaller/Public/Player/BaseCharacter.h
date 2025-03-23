// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UBGCHealthComponent;
class ASFBaseWeapon;
class USFWeaponComponent;

UCLASS()
class SKYFALLER_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE bool IsMoving() const { return !GetVelocity().IsNearlyZero() && !GetCharacterMovement()->IsFalling(); }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetMovementDirection() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Death", meta = (ClapMin = "0"))
	float LifeSpanOnDeath = 5.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UBGCHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USFWeaponComponent* WeaponComponent;

	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnDeath();
};
