// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BGCCharacterMovementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartRunning);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopRunning);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLanded);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BASEGAMECOMPONENTS_API UBGCCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	/** Called when the character starts running.*/
	UPROPERTY(BlueprintAssignable)
	FOnStartRunning OnStartRunning;

	/** Called when the character stops running.*/
	UPROPERTY(BlueprintAssignable)
	FOnStopRunning OnStopRunning;

	/** Called when the character lands on the ground.*/
	UPROPERTY(BlueprintAssignable)
	FOnLanded OnLanded;

	/** The multiplier for the character's speed when running.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement",
		meta = (ClampMin = "1.1", ClampMax = "10", UIMin = "1.1", UIMax = "10"))
	float RunMultiplier = 2.0f;

	// Is the character wants to run.
	uint8 bWantsToRun : 1;

	/**
	 * Check if the character is running.
	 * @return Is the character running.
	 */
	UFUNCTION(BlueprintPure, Category = "Movement")
	bool IsRunning() const { return bWantsToRun && IsMoving(); }

	/**
	 * Check if the character is moving.
	 * @return Is the character moving.
	 */
	UFUNCTION(BlueprintPure, Category = "Movement")
	bool IsMoving() const { return !Velocity.IsZero() && !Acceleration.IsZero() && !IsFalling(); }

	virtual float GetMaxSpeed() const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Cached running state from the last frame.
	uint8 bRunningLastFrame : 1;

	// Cached falling state from the last frame.
	uint8 bFallingLastFrame : 1;
};
