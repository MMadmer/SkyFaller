// Base game components project. All rights reserved.


#include "Components/BGCCharacterMovementComponent.h"

void UBGCCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Run logic
	const bool CachedRunning = IsRunning();
	if (bRunningLastFrame != CachedRunning)
	{
		bRunningLastFrame = CachedRunning;

		if (CachedRunning) OnStartRunning.Broadcast();
		else OnStopRunning.Broadcast();
	}

	// Falling logic
	const bool CachedFalling = IsFalling();
	if (bFallingLastFrame != CachedFalling)
	{
		bFallingLastFrame = CachedFalling;
		if (!CachedFalling) OnLanded.Broadcast();
	}
}

float UBGCCharacterMovementComponent::GetMaxSpeed() const
{
	const float MaxSpeed = Super::GetMaxSpeed();

	return IsRunning() ? MaxSpeed * RunMultiplier : MaxSpeed;
}
