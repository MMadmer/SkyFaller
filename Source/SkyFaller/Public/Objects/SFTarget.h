// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFTarget.generated.h"

class UStaticMeshComponent;

UCLASS()
class SKYFALLER_API ASFTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	ASFTarget();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Points system")
	float RewardPoints = 10.0f;

	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
