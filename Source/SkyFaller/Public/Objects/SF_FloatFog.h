// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SF_FloatFog.generated.h"

class UStaticMeshComponent;

UCLASS()
class SKYFALLER_API ASF_FloatFog : public AActor
{
	GENERATED_BODY()
	
public:	
	ASF_FloatFog();

	UFUNCTION()
	void Mover(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* FogMesh;

	virtual void BeginPlay() override;

private:
	FVector GetPlayerLocation() const;
};
