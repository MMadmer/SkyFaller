#pragma once

#include "SFCoreTypes.generated.h"

// Health

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, NewHealth, float, HealthDelta);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBarriersChangedSignature, uint8, NewBarriers, uint8, BarriersDelta);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChangedSignature, int32, Value);

USTRUCT()
struct FCoreTypes
{
	GENERATED_USTRUCT_BODY()
};
