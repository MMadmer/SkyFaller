// Sky Faller. All rights reserved.


#include "Player/Weapon/SFBowWeapon.h"

#include "SFPlayerState.h"
#include "Player/Weapon/SFArrow.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Player/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBow, All, All)

ASFBowWeapon::ASFBowWeapon(): PlayerAimAnimMontage(nullptr)
{
	
}

void ASFBowWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponMesh->SetAnimInstanceClass(AnimationInst);
}

void ASFBowWeapon::StartFire()
{
	const auto Player = GetPlayer();
	if (!Player) return;

	CachedPlayerBP = Player->GetMesh()->AnimClass; // Caching original(last) player anim BP

	if (Player->GetMovementComponent()->IsFalling()) return; // Don't start if falling

	// Set weapon aiming BP to player
	Player->GetMesh()->SetAnimInstanceClass(PlayerAimBP);

	if (!PlayerAimAnimMontage) return;
	
	const float MontageTime = ChargeTime / PlayerAimAnimMontage->SequenceLength;
	// UE_LOG(LogBow, Display, TEXT("Speed modifier %f"), MontageTime);
	// UE_LOG(LogBow, Display, TEXT("Anim length %f"), PlayerAimAnimMontage->SequenceLength);
	Player->PlayAnimMontage(PlayerAimAnimMontage, MontageTime);
	// Start weapon charging
	if (!GetWorld()) return;
	GetWorld()->GetTimerManager().SetTimer(ChargeTimer, this, &ASFBowWeapon::Charging, ChargeSpeed, true);
}

void ASFBowWeapon::StopFire()
{
	if (!GetWorld()) return;
	GetWorld()->GetTimerManager().ClearTimer(ChargeTimer);

	Charge = 0.0f;
	const auto Player = GetPlayer();
	Player->GetMesh()->SetAnimInstanceClass(CachedPlayerBP); // Returning back player cached anim BP

	if (!CanFire()) return;
	bCharged = false;

	MakeShot();
	SeriesCalc();
}

void ASFBowWeapon::MakeShot()
{
	// Get shot direction
	if (!GetWorld()) return;
	FVector TraceStart, TraceEnd;
	if (!GetTraceData(TraceStart, TraceEnd)) return;

	FHitResult HitResult;
	MakeHit(HitResult, TraceStart, TraceEnd);

	const FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
	const FVector Direction = (EndPoint - GetMuzzleWorldLocation()).GetSafeNormal();

	// Set new arrow transform(size, location, rotation)
	FRotator NewRotation = Direction.Rotation();
	NewRotation.Pitch -= 90.0f;
	FTransform SpawnTransform(NewRotation, GetMuzzleWorldLocation());
	SpawnTransform.SetScale3D(FVector(0.002f, 0.002f, 0.0015f));

	ASFArrow* Arrow = GetWorld()->SpawnActorDeferred<ASFArrow>(ProjectileClass, SpawnTransform);
	if (!Arrow) return;

	Arrow->SetShotDirection(Direction);
	Arrow->SetOwner(GetOwner());
	Arrow->FinishSpawning(SpawnTransform);

	// Shot sound
	UAudioComponent* AudioComponent = NewObject<UAudioComponent>(this);
	if (!(AudioComponent && ShotSound)) return;
	AudioComponent->SetSound(ShotSound);
	AudioComponent->Play();
}

bool ASFBowWeapon::CanFire() const
{
	return bCharged;
}

void ASFBowWeapon::Charging()
{
	// UE_LOG(LogBow, Display, TEXT("Charging: %f %%"), GetCharge());
	if (Charge >= ChargeTime)
	{
		if (!GetWorld()) return;
		GetWorld()->GetTimerManager().ClearTimer(ChargeTimer);
		bCharged = true;
		GetPlayer()->StopAnimMontage(PlayerAimAnimMontage);
	}
	else
	{
		// Stop charging if falling
		if (GetPlayer()->GetCharacterMovement()->IsFalling())
		{
			StopFire();
		}
		Charge += ChargeSpeed;
		OnChargeChanged.Broadcast(
			FMath::GetMappedRangeValueClamped(FVector2D(0.0f, ChargeTime), FVector2D(0.0f, 1.0f), Charge));
	}
}

void ASFBowWeapon::BowstringOffset(float Offset) const
{
	WeaponMesh->GlobalAnimRateScale = 1.0f;
}

void ASFBowWeapon::SeriesCalc() const
{
	const auto Player = GetPlayer();
	if (!Player) return;
	ASFPlayerState* PlayerState = Cast<ASFPlayerState>(GetPlayer()->GetPlayerState());
	if (!PlayerState) return;

	if (!PlayerState->GetSeries()) return;

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
