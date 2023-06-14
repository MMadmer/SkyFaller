// Sky Faller. All rights reserved.


#include "Player/Weapon/SFBowWeapon.h"
#include "Player/Weapon/SFArrow.h"
#include "..\..\..\Public\Player\Weapon\SFBowWeapon.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimSingleNodeInstance.h"

DEFINE_LOG_CATEGORY_STATIC(LogBow, All, All)

void ASFBowWeapon::StartFire()
{
	Charging();
}

void ASFBowWeapon::StopFire()
{
	if (!CanFire()) return;
	bCharged = false;

	MakeShot();
}

void ASFBowWeapon::MakeShot()
{
	if (!GetWorld()) return;
	FVector TraceStart, TraceEnd;
	if (!GetTraceData(TraceStart, TraceEnd)) return;

	FHitResult HitResult;
	MakeHit(HitResult, TraceStart, TraceEnd);

	const FVector EndPoint = TraceEnd; // HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
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
}

bool ASFBowWeapon::CanFire() const
{
	return bCharged;
}

void ASFBowWeapon::Charging()
{
	

	bCharged = true;
}

void ASFBowWeapon::BowstringOffset(float Offset)
{
	WeaponMesh->GlobalAnimRateScale = 1.0f;
}
