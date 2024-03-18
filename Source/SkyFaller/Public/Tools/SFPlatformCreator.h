// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "SFPlatformCreator.generated.h"

UCLASS()
class SKYFALLER_API ASFPlatformCreator : public AActor
{
	GENERATED_BODY()

public:
	ASFPlatformCreator();

protected:
	UPROPERTY(EditAnywhere, Category="Editor Tools")
	USphereComponent* Handler;

	UFUNCTION(CallInEditor, Category="Editor Tools")
	void ConvertToPlatform();

	virtual void BeginPlay() override;
};
