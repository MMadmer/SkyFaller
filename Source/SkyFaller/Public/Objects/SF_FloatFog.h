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

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* FogMesh;

	UFUNCTION()
	void KillActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	               int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;
};
