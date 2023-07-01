// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Objects/SFTrap.h"
#include "SFMineTrap.generated.h"

class USFExplosionComponent;
class URadialForceComponent;

UCLASS()
class SKYFALLER_API ASFMineTrap : public ASFTrap
{
	GENERATED_BODY()
	
public:
	ASFMineTrap();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USFExplosionComponent* ExplosionComponent;

	virtual void BeginPlay() override;

private:
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
