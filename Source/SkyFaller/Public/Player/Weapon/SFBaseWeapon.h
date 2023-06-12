// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFBaseWeapon.generated.h"

class USkeletalMeshComponent;

UCLASS()
class SKYFALLER_API ASFBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ASFBaseWeapon();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;

};
