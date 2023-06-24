// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SFCoreTypes.h"
#include "SFProgressComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SKYFALLER_API USFProgressComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	FOnScoreChangedSignature OnScoreChanged;

	USFProgressComponent();

	int32 GetScore() const { return Score; };
	void AddScore(int32 Value);

protected:
	virtual void BeginPlay() override;

private:
	int32 Score = 0;

};
