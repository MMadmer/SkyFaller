#pragma once

#include "SFCoreTypes.generated.h"

// Health

DECLARE_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnScoreChangedSignature, int32);

USTRUCT()
struct FCoreTypes
{
	GENERATED_USTRUCT_BODY()
};
