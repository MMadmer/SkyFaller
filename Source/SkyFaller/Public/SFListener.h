// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFListener.generated.h"

UCLASS()
class SKYFALLER_API ASFListener : public AActor
{
	GENERATED_BODY()
	
public:	
	ASFListener();

	UFUNCTION()
	void OnPlatformHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	virtual void BeginPlay() override;

private:
	void FogConnecting(const ACharacter* Player);

};
