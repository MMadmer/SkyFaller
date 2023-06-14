// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFArrow.generated.h"

class USkeletalMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class SKYFALLER_API ASFArrow : public AActor
{
	GENERATED_BODY()
	
public:	
	ASFArrow();

	void SetShotDirection(const FVector& Direction) { ShotDirection = Direction; }

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* ArrowMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UProjectileMovementComponent* MovementComponent;

	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void ConnectToActor(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FVector ShotDirection;
	bool bAttached = false;

};
