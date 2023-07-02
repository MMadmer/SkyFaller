// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SFExplosionComponent.generated.h"

class URadialForceComponent;
class USphereComponent;
class UNiagaraSystem;
class USoundCue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SKYFALLER_API USFExplosionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USFExplosionComponent();

	void Explode(const FVector& ImpactLocation);
	float GetInnerRadius() const { return InnerRad; };
	float GetOuterRadius() const { return OuterRad; };

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URadialForceComponent* RadialForceComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	float Damage = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	float MinDamage = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	float InnerRad = 250.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Radial Force")
	float OuterRad = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Radial Force")
	float ExplosionForce = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Radial Force")
	float ExplosionImpulse = 10000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* NiagaraSystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds", meta = (ClampMin = "0"))
	float SoundRadius = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds")
	USoundCue* ExplosionSound;

	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void OnExplosionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	void RadialDamage();

};
