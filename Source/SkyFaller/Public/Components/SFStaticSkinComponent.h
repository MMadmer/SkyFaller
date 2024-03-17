// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SFStaticSkinComponent.generated.h"

class ASFPlatformSkin;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKYFALLER_API USFStaticSkinComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USFStaticSkinComponent();

	ASFPlatformSkin* GetCurrentSkin() const { return CurrentSkin; };

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Skin")
	TSubclassOf<ASFPlatformSkin> SkinClass;

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	ASFPlatformSkin* CurrentSkin;

	void SpawnSkin();
};
