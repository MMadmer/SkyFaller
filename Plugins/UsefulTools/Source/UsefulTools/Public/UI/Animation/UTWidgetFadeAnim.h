// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Tickable.h"
#include "UTWidgetFadeAnim.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnimComplete, const bool, FromFadeIn);

class UWidget;

/**
 * Widget fade animation. Should be one per each widget.
 */
UCLASS()
class USEFULTOOLS_API UUTWidgetFadeAnim final : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:
	explicit UUTWidgetFadeAnim(const FObjectInitializer& ObjectInitializer);

	UPROPERTY()
	FOnAnimComplete OnAnimComplete;

	static UUTWidgetFadeAnim* FindWidgetShowHideAnim(const UWidget* Widget);

	void Reset();

	void SetAnimationLength(const float Length);
	float GetAnimationLength() const { return AnimLength; }
	float GetCurrentTime() const { return CurrentTime; }

	void PlayAnimation(const bool Forward);
	void PlayFromStart();
	void PlayFromEnd();
	bool IsPlaying() const { return IsTickable(); }
	bool IsFadeIn() const { return bFadeIn; }

	virtual void PostInitProperties() override;

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return bCanEverTick; }
	virtual TStatId GetStatId() const override { return TStatId(); }

private:
	float AnimLength = 0.1f;
	uint8 bFadeIn : 1;
	uint8 bCanEverTick : 1;

	TWeakObjectPtr<UWidget> OwningWidget;
	float CurrentTime = 0.0f;
};
