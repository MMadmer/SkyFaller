// Sky Faller. All rights reserved.


#include "Player/Weapon/SFBowWeapon.h"

#include "Player/SFPlayerState.h"
#include "Player/Weapon/SFArrow.h"
#include "Animation/AnimInstance.h"
#include "Player/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void ASFBowWeapon::StartFire_Implementation()
{
	const auto Player = GetPlayer();
	if (!Player) return;

	const UPawnMovementComponent* MovementComp = Player->GetMovementComponent();
	if (MovementComp->IsFalling()) return; // Don't start if falling

	if (IsValid(PlayerAimAnimMontage))
	{
		const float MontageTime = ChargeTime / PlayerAimAnimMontage->SequenceLength;
		Player->PlayAnimMontage(PlayerAimAnimMontage, MontageTime);
	}

	// Start weapon charging
	if (!GetWorld()) return;
	GetWorld()->GetTimerManager().SetTimer(ChargeTimer, this, &ASFBowWeapon::Charging, ChargeSpeed, true);
}

void ASFBowWeapon::StopFire_Implementation()
{
	if (!GetWorld()) return;
	GetWorld()->GetTimerManager().ClearTimer(ChargeTimer);

	Charge = 0.0f;

	if (!CanFire()) return;
	bCharged = false;

	MakeShot();
	SeriesCalc();
}

bool ASFBowWeapon::MakeShot_Implementation()
{
	UWorld* World = GetWorld();
	if (!World) return false;

	// Get shot direction
	FVector TraceStart, TraceEnd;
	if (!GetTraceData(TraceStart, TraceEnd)) return false;

	FHitResult HitResult;
	MakeHit(HitResult, TraceStart, TraceEnd);

	const FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
	const FVector Direction = (EndPoint - GetMuzzleWorldLocation()).GetSafeNormal();

	// Set new arrow transform(size, location, rotation)
	FRotator NewRotation = Direction.Rotation();
	NewRotation.Pitch -= 90.0f;
	FTransform SpawnTransform(NewRotation, GetMuzzleWorldLocation());
	SpawnTransform.SetScale3D(FVector(0.002f, 0.002f, 0.0015f));

	ASFArrow* Arrow = World->SpawnActorDeferred<ASFArrow>(ProjectileClass, SpawnTransform);
	if (!Arrow) return false;

	Arrow->SetShotDirection(Direction);
	Arrow->SetOwner(GetOwner());
	Arrow->FinishSpawning(SpawnTransform);

	return true;
}

void ASFBowWeapon::Charging()
{
	if (Charge >= ChargeTime)
	{
		const UWorld* World = GetWorld();
		if (!World) return;

		World->GetTimerManager().ClearTimer(ChargeTimer);
		bCharged = true;
		GetPlayer()->StopAnimMontage(PlayerAimAnimMontage);
	}
	else
	{
		// Stop charging if falling
		if (GetPlayer()->GetCharacterMovement()->IsFalling()) StopFire();

		Charge += ChargeSpeed;
		OnChargeChanged.Broadcast(
			FMath::GetMappedRangeValueClamped(FVector2D(0.0f, ChargeTime), FVector2D(0.0f, 1.0f), Charge));
	}
}

void ASFBowWeapon::SeriesCalc() const
{
	const auto Player = GetPlayer();
	if (!Player) return;

	ASFPlayerState* PlayerState = Cast<ASFPlayerState>(GetPlayer()->GetPlayerState());
	if (!PlayerState) return;

	if (PlayerState->GetSeries() == 0) return;

	if (PlayerState->bInSeries)
	{
		PlayerState->SetSeries(0);
		PlayerState->bInSeries = false;
	}
	else
	{
		PlayerState->bInSeries = true;
	}
}
