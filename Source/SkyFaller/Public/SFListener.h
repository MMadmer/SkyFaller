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

protected:
	virtual void BeginPlay() override;

};
