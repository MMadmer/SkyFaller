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

	virtual void Tick(float DeltaTime) override;

	void SetShotDirection(const FVector& Direction) { ShotDirection = Direction; }

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* ArrowMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UProjectileMovementComponent* MovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0", ClampMax = "1"))
	float PercentagePenetration = 0.19643f;


	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void ConnectToActor(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FVector ShotDirection;
	bool bAttached = false;
	float ArrowLenght = 112.0f;

	void PhysicsFalling();

};
