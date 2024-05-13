#pragma once

#include "SFCoreTypes.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChangedSignature, int32, Value);

USTRUCT()
struct FCoreTypes
{
	GENERATED_USTRUCT_BODY()
};
