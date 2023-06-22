#pragma once

// Health

DECLARE_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int32);
