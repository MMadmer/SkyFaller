// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UTWidgetFadeAnim.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "UTPlayWidgetFadeAnim.generated.h"

class UWidget;

/**
 * 
 */
UCLASS()
class USEFULTOOLS_API UUTPlayWidgetFadeAnim : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnAnimComplete OnAnimComplete;

	/** Play widget fade animation from internal current animation time.*/
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, meta=(AdvancedDisplay="AnimationLength"))
	static UUTPlayWidgetFadeAnim* FadeWidget(UWidget* Widget, const bool FadeIn = true,
	                                       const float AnimationLength = 0.1f);

	/** Play toggled widget fade animation from internal current animation time.*/
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, meta=(AdvancedDisplay="AnimationLength"))
	static UUTPlayWidgetFadeAnim* ToggleWidgetFade(UWidget* Widget, const float AnimationLength = 0.1f);

	/** Play fade in widget animation from start.*/
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, meta=(AdvancedDisplay="AnimationLength"))
	static UUTPlayWidgetFadeAnim* FadeInWidgetFromStart(UWidget* Widget, const float AnimationLength = 0.1f);

	/** Play fade out widget animation from end.*/
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, meta=(AdvancedDisplay="AnimationLength"))
	static UUTPlayWidgetFadeAnim* FadeOutWidgetFromEnd(UWidget* Widget, const float AnimationLength = 0.1f);

protected:
	virtual void Activate() override;

private:
	TWeakObjectPtr<UWidget> AnimWidget;
	TWeakObjectPtr<UUTWidgetFadeAnim> Animation;
	uint8 bFadeIn : 1;
	uint8 bSwitch : 1;
	uint8 bReset : 1;
	float AnimLength;

	UFUNCTION()
	void OnAnimationComplete(const bool FromFadeIn);
};
