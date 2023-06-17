// Sky Faller. All rights reserved.


#include "UI/SFGameHUD.h"
#include "UI/SFPlayerWidget.h"
#include "Blueprint/UserWidget.h"

void ASFGameHUD::DrawHUD()
{
	Super::DrawHUD();
}

void ASFGameHUD::BeginPlay()
{
	Super::BeginPlay();

	auto PlayerWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHudWidgetClass);
	if (PlayerWidget)
	{
		PlayerWidget->AddToViewport();
	}
}
