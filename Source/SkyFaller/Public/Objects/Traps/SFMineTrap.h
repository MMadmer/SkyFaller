// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Traps/SFTrap.h"
#include "SFMineTrap.generated.h"

class USFExplosionComponent;
class URadialForceComponent;
class USphereComponent;
class USoundCue;

UCLASS()
class SKYFALLER_API ASFMineTrap : public ASFTrap
{
	GENERATED_BODY()

public:
	ASFMineTrap();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	USFExplosionComponent* ExplosionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USphereComponent* InnerSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USphereComponent* OuterSphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sounds")
	USoundCue* TickSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sounds", meta = (ClampMin="0", UIMin="0"))
	float TimerTime = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sounds", meta = (ClampMin="0", UIMin="0"))
	float TimerTick = 0.1f;

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

private:
	FTimerHandle TimerHandle;
	bool bExploded = false;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	           FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnInnerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOuterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void Explosion();
	void Timer();
};
