// Sky Faller. All rights reserved.


#include "Player/SFPlayerState.h"

ASFPlayerState::ASFPlayerState()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bInSeries = false;
}

void ASFPlayerState::AddScore(const int32 Value)
{
	Score = FMath::Max(Score + Value, 0);
	OnScoreChanged.Broadcast(Value);
}
