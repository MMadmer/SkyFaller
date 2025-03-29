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

	UPROPERTY(BlueprintReadWrite)
	uint8 bInSeries : 1;

	UFUNCTION(BlueprintPure)
	int32 GetPlayerScore() const { return Score; }

	UFUNCTION(BlueprintCallable)
	void AddScore(const int32 Value);

	UFUNCTION(BlueprintPure)
	int32 GetSeries() const { return Series; }

	UFUNCTION(BlueprintCallable)
	void SetSeries(const int32 Value) { Series = FMath::Max(Value, 0); }

protected:
	int32 Score = 0;
	int32 Series = 0;
};
