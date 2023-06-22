// Sky Faller. All rights reserved.


#include "Components/SFProgressComponent.h"

USFProgressComponent::USFProgressComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void USFProgressComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void USFProgressComponent::AddScore(int32 Value)
{ 
	Score += Value;
	OnScoreChanged.Broadcast(Value);
}
