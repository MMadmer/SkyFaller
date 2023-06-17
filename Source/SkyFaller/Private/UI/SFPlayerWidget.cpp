// Sky Faller. All rights reserved.


#include "UI/SFPlayerWidget.h"
#include "Components/SFProgressComponent.h"

int32 USFPlayerWidget::GetPlayerScore() const
{
	const auto Player = GetOwningPlayerPawn();
	if (!Player) return 0;

	const auto ProgressComponent = Cast<USFProgressComponent>(Player->GetComponentByClass(USFProgressComponent::StaticClass()));
	if (!ProgressComponent) return 0;

	return ProgressComponent->GetScore();
}
