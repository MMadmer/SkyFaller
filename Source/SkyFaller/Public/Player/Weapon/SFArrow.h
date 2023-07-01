// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFArrow.generated.h"

class USkeletalMeshComponent;
class UProjectileMovementComponent;
class USoundCue;

UCLASS()
class SKYFALLER_API ASFArrow : public AActor
{
	GENERATED_BODY()
	
public:	
	ASFArrow();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physics", meta = (ClapMin = "0"))
	float ImpactForceMultiplier = 1.0f;

	void SetShotDirection(const FVector& Direction) { ShotDirection = Direction; }
	float GetLifeHitTrace() const { return LifeHitTrace; };
	FVector GetVelocity() const;
	USkeletalMeshComponent* GetMesh() const { return ArrowMesh; };

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* ArrowMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UProjectileMovementComponent* MovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physics", meta = (ClampMin = "0", ClampMax = "1"))
	float PercentagePenetration = 0.19643f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physics", meta = (ClampMin = "0"))
	float Mass = 0.15f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundCue* HitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound", meta = (ClampMin = "0"))
	float HitSoundRadius = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX")
	float LifeHitTrace = 2.5f;

	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FVector ShotDirection;
	bool bAttached = false;
	FTimerHandle TraceTimer;

	void PhysicsFalling();

};
