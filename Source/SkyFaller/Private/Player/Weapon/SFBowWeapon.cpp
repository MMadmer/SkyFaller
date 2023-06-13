// Sky Faller. All rights reserved.


#include "Player/Weapon/SFBowWeapon.h"
#include "Player/Weapon/SFArrow.h"
#include "..\..\..\Public\Player\Weapon\SFBowWeapon.h"

DEFINE_LOG_CATEGORY_STATIC(LogBow, All, All)

void ASFBowWeapon::StartFire()
{
	MakeShot();
}

void ASFBowWeapon::MakeShot()
{
	if (!GetWorld()) return;
	FVector TraceStart, TraceEnd;
	if (!GetTraceData(TraceStart, TraceEnd)) return;

	FHitResult HitResult;
	MakeHit(HitResult, TraceStart, TraceEnd);

	const FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
	const FVector Direction = (EndPoint - GetMuzzleWorldLocation()).GetSafeNormal();

	const FTransform SpawnTransform(FRotator::ZeroRotator, GetMuzzleWorldLocation());
	ASFArrow* Arrow = GetWorld()->SpawnActorDeferred<ASFArrow>(ProjectileClass, SpawnTransform);
	if (!Arrow) return;

	Arrow->SetShotDirection(Direction);
	Arrow->SetOwner(GetOwner());
	Arrow->FinishSpawning(SpawnTransform);
}
