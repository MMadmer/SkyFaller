// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Animation/UTPlayWidgetFadeAnim.h"

#include "Components/Widget.h"

UUTPlayWidgetFadeAnim* UUTPlayWidgetFadeAnim::FadeWidget(UWidget* Widget, const bool FadeIn,
                                                         const float AnimationLength)
{
	UUTPlayWidgetFadeAnim* PlayWidgetShowHideAnim = NewObject<UUTPlayWidgetFadeAnim>();
	PlayWidgetShowHideAnim->AnimWidget = Widget;
	PlayWidgetShowHideAnim->bFadeIn = FadeIn;
	PlayWidgetShowHideAnim->AnimLength = AnimationLength;

	return PlayWidgetShowHideAnim;
}

UUTPlayWidgetFadeAnim* UUTPlayWidgetFadeAnim::ToggleWidgetFade(UWidget* Widget, const float AnimationLength)
{
	UUTPlayWidgetFadeAnim* PlayWidgetShowHideAnim = NewObject<UUTPlayWidgetFadeAnim>();
	PlayWidgetShowHideAnim->AnimWidget = Widget;
	PlayWidgetShowHideAnim->bFadeIn = true;
	PlayWidgetShowHideAnim->bSwitch = true;
	PlayWidgetShowHideAnim->AnimLength = AnimationLength;

	return PlayWidgetShowHideAnim;
}

UUTPlayWidgetFadeAnim* UUTPlayWidgetFadeAnim::FadeInWidgetFromStart(UWidget* Widget, const float AnimationLength)
{
	UUTPlayWidgetFadeAnim* PlayWidgetShowHideAnim = NewObject<UUTPlayWidgetFadeAnim>();
	PlayWidgetShowHideAnim->AnimWidget = Widget;
	PlayWidgetShowHideAnim->bFadeIn = true;
	PlayWidgetShowHideAnim->bReset = true;
	PlayWidgetShowHideAnim->AnimLength = AnimationLength;

	return PlayWidgetShowHideAnim;
}

UUTPlayWidgetFadeAnim* UUTPlayWidgetFadeAnim::FadeOutWidgetFromEnd(UWidget* Widget, const float AnimationLength)
{
	UUTPlayWidgetFadeAnim* PlayWidgetShowHideAnim = NewObject<UUTPlayWidgetFadeAnim>();
	PlayWidgetShowHideAnim->AnimWidget = Widget;
	PlayWidgetShowHideAnim->bFadeIn = false;
	PlayWidgetShowHideAnim->bReset = true;
	PlayWidgetShowHideAnim->AnimLength = AnimationLength;

	return PlayWidgetShowHideAnim;
}

void UUTPlayWidgetFadeAnim::Activate()
{
	Super::Activate();

	if (!AnimWidget.IsValid()) return;
	UWidget* Widget = AnimWidget.Get();

	// Find or create animation
	UUTWidgetFadeAnim* Anim = UUTWidgetFadeAnim::FindWidgetShowHideAnim(Widget);
	if (!IsValid(Anim)) Anim = NewObject<UUTWidgetFadeAnim>(Widget);
	Animation = Anim;
	if (!Animation.IsValid()) return;

	Anim->SetAnimationLength(AnimLength);

	// Switch play direction if needed
	if (bSwitch) bFadeIn = !Anim->IsFadeIn();

	// If play from end or start
	if (bReset)
	{
		if (bFadeIn) Anim->PlayFromStart();
		else Anim->PlayFromEnd();
	}

	// Assign on complete exec pin
	Anim->OnAnimComplete.AddDynamic(this, &ThisClass::OnAnimationComplete);

	Anim->PlayAnimation(bFadeIn);
}

void UUTPlayWidgetFadeAnim::OnAnimationComplete(const bool FromFadeIn)
{
	if (Animation.IsValid()) Animation->OnAnimComplete.RemoveDynamic(this, &ThisClass::OnAnimationComplete);
	OnAnimComplete.Broadcast(FromFadeIn);
}
