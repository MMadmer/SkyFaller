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
	/** Platform's static mesh actor tag. */
	UPROPERTY(EditAnywhere, Category="Editor Tools")
	FName PlatformTag;

	/** Working radius. */
	UPROPERTY(EditAnywhere, Category="Editor Tools")
	USphereComponent* Handler;

	/** Convert all actors to platform's static mesh sockets(set PlatformTag to target platform static mesh actor). */
	UFUNCTION(CallInEditor, Category="Editor Tools")
	void ConvertToPlatform();

	/** Clear all platform sockets(set PlatformTag to target platform static mesh actor). */
	UFUNCTION(CallInEditor, Category="Editor Tools")
	void ClearPlatform() const;

	virtual void BeginPlay() override;

	void FindAndRemoveActorByTag(TArray<AActor*>& Actors, AActor*& Platform) const;
};
