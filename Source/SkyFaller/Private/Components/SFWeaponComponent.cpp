// Sky Faller. All rights reserved.


#include "Components/SFWeaponComponent.h"
#include "Player/Weapon/SFBaseWeapon.h"
#include "Player/BaseCharacter.h"
#include "..\..\Public\Components\SFWeaponComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All)

USFWeaponComponent::USFWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USFWeaponComponent::StartFire()
{
	if (!CanFire()) return;

	CurrentWeapon->StartFire();
}

void USFWeaponComponent::StopFire()
{
	CurrentWeapon->StopFire();
}

void USFWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();
}

void USFWeaponComponent::SpawnWeapon()
{
	const auto World = GetWorld();
	if (!World) return;
	const auto Player = Cast<ACharacter>(GetOwner());
	if (!Player) return;

	auto Weapon = World->SpawnActor<ASFBaseWeapon>(WeaponClass);
	if (!Weapon) return;

	// All checks done. Set weapon to current
	Weapon->SetOwner(Player);
	CurrentWeapon = Weapon;

	AttachWeaponToSocket(CurrentWeapon, Player->GetMesh(), WeaponEquipSocketName);
}

void USFWeaponComponent::AttachWeaponToSocket(ASFBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName)
{
	if (!Weapon || !SceneComponent) return;

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
	Weapon->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
}

bool USFWeaponComponent::CanFire()
{
	return !CurrentWeapon ? false : true;
}
