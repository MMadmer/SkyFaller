// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Components/ActorComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Sound/SoundCue.h"
#include "SFExplosionComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKYFALLER_API USFExplosionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USFExplosionComponent();

	void Explode(const FVector& ImpactLocation);
	FORCEINLINE float GetInnerRadius() const { return InnerRad; }
	FORCEINLINE float GetOuterRadius() const { return OuterRad; }
	FORCEINLINE float GetDamage() const { return Damage; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	URadialForceComponent* RadialForceComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USphereComponent* SphereComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Data")
	float Damage = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Data",
		meta=(ClampMin="0", ClampMax="1", UIMin="0", UIMax="1"))
	float MinDamage = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Data")
	float InnerRad = 150.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Radial Force")
	float OuterRad = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Radial Force")
	float ExplosionForce = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Radial Force")
	float ExplosionImpulse = 50000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="VFX")
	UNiagaraSystem* NiagaraSystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sounds", meta=(ClampMin="0", UIMin="0"))
	float SoundRadius = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sounds")
	USoundCue* ExplosionSound;

	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnExplosionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                        const FHitResult& SweepResult);


	void RadialDamage() const;
};
