// Sky Faller. All rights reserved.


#include "Player/Weapon/SFBowWeapon.h"
#include "Player/Weapon/SFArrow.h"
#include "..\..\..\Public\Player\Weapon\SFBowWeapon.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Player/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SFProgressComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBow, All, All)

void ASFBowWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponMesh->SetAnimInstanceClass(AnimationInst);
}

void ASFBowWeapon::StartFire()
{
	const auto Player = GetPlayer();
	ChachedPlayerBP = Player->GetMesh()->AnimClass; // Caching original(last) player anim BP
	if (Player->GetMovementComponent()->IsFalling()) return; // Don't start if falling
	// Set weapon aiming BP to player
	Player->GetMesh()->SetAnimInstanceClass(PlayerAimBP);
	float MontageTime = ChargeTime / PlayerAimAnimMontage->SequenceLength;
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
	Player->GetMesh()->SetAnimInstanceClass(ChachedPlayerBP); // Returning back player cached anim BP

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
	}
}

void ASFBowWeapon::BowstringOffset(float Offset)
{
	WeaponMesh->GlobalAnimRateScale = 1.0f;
}

void ASFBowWeapon::SeriesCalc()
{
	const auto Player = GetPlayer();
	if (!Player) return;
	USFProgressComponent* ProgressComponent = Cast<USFProgressComponent>(Player->GetComponentByClass(USFProgressComponent::StaticClass()));
	if (!ProgressComponent) return;

	if (!ProgressComponent->GetSeries()) return;

	if (ProgressComponent->bInSeries)
	{
		ProgressComponent->SetSeries(0);
		ProgressComponent->bInSeries = false;
	}
	else
	{
		ProgressComponent->bInSeries = true;
	}
}
