// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SFGameHUD.generated.h"

UCLASS()
class SKYFALLER_API ASFGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

protected:
	virtual void BeginPlay() override;
};
