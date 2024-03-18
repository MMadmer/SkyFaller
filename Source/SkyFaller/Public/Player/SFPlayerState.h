// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SFCoreTypes.h"
#include "SFPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SKYFALLER_API ASFPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASFPlayerState();

	UPROPERTY(BlueprintAssignable)
	FOnScoreChangedSignature OnScoreChanged;

	bool bInSeries = false;
	int32 GetScore() const { return Score; };
	void AddScore(const int32 Value);
	int32 GetSeries() const { return Series; };
	void SetSeries(const int32 Value) { Series = Value; };

protected:
	int32 Score = 0;
	int32 Series = 0;
};
