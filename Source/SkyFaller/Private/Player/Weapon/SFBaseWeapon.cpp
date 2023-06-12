// Sky Faller. All rights reserved.


#include "Player/Weapon/SFBaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"

ASFBaseWeapon::ASFBaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);
}

void ASFBaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}
