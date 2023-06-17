// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SFSaveGame.generated.h"

UCLASS()
class SKYFALLER_API USFSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	int32 Skills;
	
};
