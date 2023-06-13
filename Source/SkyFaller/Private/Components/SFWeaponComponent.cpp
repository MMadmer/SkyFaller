// Sky Faller. All rights reserved.


#include "Components/SFWeaponComponent.h"
#include "Player/Weapon/SFBaseWeapon.h"
#include "Player/BaseCharacter.h"

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

}

void USFWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();
	CurrentWeapon = WeaponClass.GetDefaultObject();
}

void USFWeaponComponent::SpawnWeapon()
{
	const auto World = GetWorld();
	if (!World) return;
	const auto Player = Cast<ACharacter>(GetOwner());
	if (!Player) return;

	auto Weapon = World->SpawnActor<ASFBaseWeapon>(WeaponClass);
	if (!Weapon) return;

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
	Weapon->AttachToComponent(Player->GetMesh(), AttachmentRules, WeaponEquipSocketName);
}

bool USFWeaponComponent::CanFire()
{
	return !CurrentWeapon ? false : true;
}
