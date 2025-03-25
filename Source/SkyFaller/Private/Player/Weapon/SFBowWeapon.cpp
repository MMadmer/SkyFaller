// Sky Faller. All rights reserved.


#include "Player/Weapon/SFBowWeapon.h"

#include "Player/SFPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

void ASFBowWeapon::StartFire_Implementation()
{
	const auto Player = Cast<ACharacter>(GetOwner());
	if (!Player) return;

	const UPawnMovementComponent* MovementComp = Player->GetMovementComponent();
	if (MovementComp->IsFalling()) return; // Don't start if falling

	if (IsValid(PlayerAimAnimMontage))
	{
		const float MontageTime = PlayerAimAnimMontage->SequenceLength / ChargeTime;
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

	ResetCharge();
}

bool ASFBowWeapon::MakeShot_Implementation()
{
	UWorld* World = GetWorld();
	if (!World || !bCharged) return false;

	// Get shot direction
	FVector TraceStart, TraceEnd;
	if (!GetTraceData(TraceStart, TraceEnd)) return false;

	FHitResult HitResult;
	MakeHit(HitResult, TraceStart, TraceEnd);

	const FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
	const FVector Direction = (EndPoint - GetMuzzleWorldLocation()).GetSafeNormal();

	// Set new arrow transform(size, location, rotation)
	FRotator NewRotation = Direction.Rotation();
	FTransform SpawnTransform(NewRotation, GetMuzzleWorldLocation());

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.Owner = this;
	AActor* Arrow = World->SpawnActor<AActor>(ProjectileClass, SpawnTransform, SpawnParams);
	if (!Arrow) return false;

	const auto ProjComp = Cast<UProjectileMovementComponent>(
		Arrow->GetComponentByClass(UProjectileMovementComponent::StaticClass()));
	if (IsValid(ProjComp)) ProjComp->Velocity = Direction * ShotStrength + GetVelocity();

	SeriesCalc();
	ResetCharge();

	return true;
}

void ASFBowWeapon::ResetCharge()
{
	Charge = 0.0f;
	bCharged = false;
}

void ASFBowWeapon::Charging()
{
	const auto Player = Cast<ACharacter>(GetOwner());
	if (Charge >= ChargeTime)
	{
		const UWorld* World = GetWorld();
		if (!World) return;

		World->GetTimerManager().ClearTimer(ChargeTimer);
		bCharged = true;
		if (IsValid(Player)) Player->StopAnimMontage(PlayerAimAnimMontage);
	}
	else
	{
		// Stop charging if falling
		if (IsValid(Player))
		{
			const UPawnMovementComponent* MovementComp = Player->GetMovementComponent();
			if (!IsValid(MovementComp) || MovementComp->IsFalling()) StopFire();
		}

		Charge += ChargeSpeed;
		OnChargeChanged.Broadcast(
			FMath::GetMappedRangeValueClamped(FVector2D(0.0f, ChargeTime), FVector2D(0.0f, 1.0f), Charge));
	}
}

void ASFBowWeapon::SeriesCalc() const
{
	const auto Player = Cast<ACharacter>(GetOwner());
	if (!IsValid(Player)) return;

	ASFPlayerState* PlayerState = Cast<ASFPlayerState>(Player->GetPlayerState());
	if (!IsValid(PlayerState)) return;

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
