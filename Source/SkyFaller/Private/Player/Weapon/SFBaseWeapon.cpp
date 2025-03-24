// Sky Faller. All rights reserved.


#include "Player/Weapon/SFBaseWeapon.h"

#include "GameFramework/Controller.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"

ASFBaseWeapon::ASFBaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);
}

void ASFBaseWeapon::OnWeaponSet_Implementation()
{
	const auto Player = GetPlayer();
	if (!IsValid(Player)) return;

	if (IsValid(PlayerAnimBP))
	{
		const auto PlayerMesh = Player->GetMesh();
		if (IsValid(PlayerMesh))
		{
			// Caching original(last) player anim BP
			CachedPlayerBP = PlayerMesh->GetAnimClass();
			PlayerMesh->SetAnimInstanceClass(PlayerAnimBP);
		}
	}
}

void ASFBaseWeapon::OnWeaponDrop_Implementation()
{
	const auto Player = GetPlayer();
	if (IsValid(Player))
	{
		USkeletalMeshComponent* PlayerMesh = Player->GetMesh();
		if (IsValid(PlayerMesh) && PlayerMesh->GetAnimClass() == PlayerAnimBP)
		{
			PlayerMesh->SetAnimInstanceClass(CachedPlayerBP);
		}
	}

	CachedPlayerBP = nullptr;
}

bool ASFBaseWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
	FVector ViewLocation;
	FRotator ViewRotation;
	if (!GetPlayerViewPoint(ViewLocation, ViewRotation)) return false;

	TraceStart = ViewLocation;
	const FVector ShootDirection = ViewRotation.Vector();
	TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;

	return true;
}

APlayerController* ASFBaseWeapon::GetPlayerController() const
{
	const auto Player = Cast<ACharacter>(GetOwner());
	if (!Player) return nullptr;

	return Player->GetController<APlayerController>();
}

bool ASFBaseWeapon::GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const
{
	const auto Controller = GetPlayerController();
	if (!Controller) return false;

	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
	return true;
}

void ASFBaseWeapon::MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd) const
{
	const UWorld* World = GetWorld();
	if (!World) return;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	CollisionParams.bReturnPhysicalMaterial = true;

	World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, CollisionParams);
}
