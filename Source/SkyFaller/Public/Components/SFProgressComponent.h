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
	bool bInSeries = false;

	USFProgressComponent();

	int32 GetScore() const { return Score; };
	void AddScore(int32 Value);
	int32 GetSeries() const { return Series; };
	void SetSeries(int32 Value) { Series = Value; };

protected:
	virtual void BeginPlay() override;

private:
	int32 Score = 0;
	int32 Series = 0;

};
