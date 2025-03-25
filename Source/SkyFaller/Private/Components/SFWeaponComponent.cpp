// Sky Faller. All rights reserved.

#include "Components/SFWeaponComponent.h"

USFWeaponComponent::USFWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USFWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();
}

void USFWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SetCurrentWeapon(nullptr);

	Super::EndPlay(EndPlayReason);
}

void USFWeaponComponent::StartFire()
{
	if (!CanFire()) return;

	OnStartFire.Broadcast();
	CurrentWeapon->StartFire();
}

void USFWeaponComponent::StopFire()
{
	if (IsValid(CurrentWeapon)) CurrentWeapon->StopFire();
	OnStopFire.Broadcast();
}

void USFWeaponComponent::SetCurrentWeapon(ASFBaseWeapon* Weapon)
{
	if (GetCurrentWeapon() == Weapon) return;

	// Drop current weapon
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->OnWeaponDrop();
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Destroy();
	}

	// Set new weapon
	CurrentWeapon = Weapon;
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->SetOwner(GetOwner());
		const auto Player = Cast<ACharacter>(GetOwner());
		if (IsValid(Player))
		{
			AttachWeaponToSocket(CurrentWeapon, Player->GetMesh(), CurrentWeapon->GetEquipSocketName());
		}
		CurrentWeapon->OnWeaponSet();
	}

	OnWeaponChanged.Broadcast(CurrentWeapon);
}

void USFWeaponComponent::SpawnWeapon()
{
	const auto World = GetWorld();
	if (!World || !WeaponClass) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.Owner = GetOwner();
	const auto Weapon = World->SpawnActor<ASFBaseWeapon>(WeaponClass, SpawnParams);
	if (!IsValid(Weapon)) return;

	SetCurrentWeapon(Weapon);
}

void USFWeaponComponent::AttachWeaponToSocket(ASFBaseWeapon* Weapon, USceneComponent* SceneComponent,
                                              const FName& SocketName)
{
	if (!Weapon || !SceneComponent) return;

	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
	Weapon->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
}
