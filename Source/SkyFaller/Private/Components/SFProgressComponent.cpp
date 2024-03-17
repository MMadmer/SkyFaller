// Sky Faller. All rights reserved.


#include "Components/SFProgressComponent.h"

USFProgressComponent::USFProgressComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USFProgressComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USFProgressComponent::AddScore(const int32 Value)
{
	Score += Value;
	OnScoreChanged.Broadcast(Value);
}
