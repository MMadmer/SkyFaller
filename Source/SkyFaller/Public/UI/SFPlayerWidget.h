// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SFPlayerWidget.generated.h"

class USFProgressComponent;

UCLASS()
class SKYFALLER_API USFPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable, Category = "UI")
	float GetWeaponCharge() const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void OnScoreChanged(const int32 Value);

	UPROPERTY(BlueprintReadWrite)
	int32 CachedScore = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 PlayerScore = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Time")
	float CachedScoreTime = 5.0f;

private:
	FTimerHandle CachedScoreTimer;

	UFUNCTION()
	void OnScoreTimerEnd();
};
