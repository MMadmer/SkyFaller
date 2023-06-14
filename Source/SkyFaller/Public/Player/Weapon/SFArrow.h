// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFArrow.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class SKYFALLER_API ASFArrow : public AActor
{
	GENERATED_BODY()
	
public:	
	ASFArrow();

	void SetShotDirection(const FVector& Direction) { ShotDirection = Direction; }

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	UStaticMeshComponent* ArrowMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	UProjectileMovementComponent* MovementComponent;

	virtual void BeginPlay() override;

private:
	FVector ShotDirection;

};
