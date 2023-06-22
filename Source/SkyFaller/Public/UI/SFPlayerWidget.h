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

public:
	void Construct();

protected:
	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable, Category = "UI")
	int32 GetPlayerScore() const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	float GetWeaponCharge() const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void OnScoreChanged(int32 Value);
	
	UFUNCTION(BlueprintCallable, Category = "Utils")
	USFProgressComponent* GetProgressComponent() const;

	UPROPERTY(BlueprintReadWrite)
	int32 CachedScore = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Time")
	float CachedScoreTime = 5.0f;

private:
	FTimerHandle CachedScoreTimer;

	UFUNCTION()
	void OnScoreTimerEnd();
};
