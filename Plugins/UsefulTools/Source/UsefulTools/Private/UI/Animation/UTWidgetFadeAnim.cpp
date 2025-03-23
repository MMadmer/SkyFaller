// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Animation/UTWidgetFadeAnim.h"

#include "Components/Widget.h"

UUTWidgetFadeAnim::UUTWidgetFadeAnim(const FObjectInitializer& ObjectInitializer)
{
	bCanEverTick = false;

	OwningWidget = Cast<UWidget>(GetOuter());
	bFadeIn = !(OwningWidget.IsValid() && (!OwningWidget->IsVisible() || FMath::IsNearlyZero(
		OwningWidget->GetRenderOpacity())));
}

void UUTWidgetFadeAnim::PostInitProperties()
{
	UObject::PostInitProperties();

	if (!OwningWidget.IsValid()) ConditionalBeginDestroy();
}

void UUTWidgetFadeAnim::Tick(float DeltaTime)
{
	if (!OwningWidget.IsValid()) return;
	UWidget* Widget = OwningWidget.Get();

	DeltaTime *= IsFadeIn() ? 1.0f : -1.0f;
	CurrentTime = FMath::Clamp(CurrentTime + DeltaTime, 0.0f, AnimLength);

	// Update widget and opacity
	//if (!Widget->IsVisible()) Widget->SetVisibility(ESlateVisibility::Visible);
	Widget->SetRenderOpacity(
		FMath::GetMappedRangeValueClamped(FVector2D(0.0f, AnimLength), FVector2D(0.0f, 1.0f), CurrentTime));

	// Check on finish animation
	if (IsFadeIn()
		    ? FMath::IsNearlyEqual(Widget->GetRenderOpacity(), 1.0f)
		    : FMath::IsNearlyZero(Widget->GetRenderOpacity()))
	{
		if (!IsFadeIn()) Widget->SetVisibility(ESlateVisibility::Collapsed);
		bCanEverTick = false;
		OnAnimComplete.Broadcast(IsFadeIn());
	}
}

UUTWidgetFadeAnim* UUTWidgetFadeAnim::FindWidgetShowHideAnim(const UWidget* Widget)
{
	if (!IsValid(Widget)) return nullptr;

	TArray<UObject*> Inners;
	GetObjectsWithOuter(Widget, Inners, false);

	UUTWidgetFadeAnim* Anim = nullptr;
	for (const auto& Inner : Inners)
	{
		Anim = Cast<UUTWidgetFadeAnim>(Inner);
		if (IsValid(Anim)) break;
	}

	return Anim;
}

void UUTWidgetFadeAnim::Reset()
{
	bCanEverTick = false;
	bFadeIn = true;
	CurrentTime = 0.0f;
}

void UUTWidgetFadeAnim::SetAnimationLength(const float Length)
{
	AnimLength = FMath::Max(Length, 0.0f);
	CurrentTime = !IsFadeIn() ? 0.0f : AnimLength;
}

void UUTWidgetFadeAnim::PlayAnimation(const bool Forward)
{
	if (!OwningWidget.IsValid()) return;

	OwningWidget.Get()->SetVisibility(ESlateVisibility::Visible);
	bFadeIn = Forward;
	bCanEverTick = true;
}

void UUTWidgetFadeAnim::PlayFromStart()
{
	Reset();
	PlayAnimation(true);
}

void UUTWidgetFadeAnim::PlayFromEnd()
{
	Reset();
	CurrentTime = AnimLength;
	PlayAnimation(false);
}
