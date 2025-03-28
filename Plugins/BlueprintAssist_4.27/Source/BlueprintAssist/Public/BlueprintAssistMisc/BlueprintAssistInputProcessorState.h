﻿// Copyright fpwong. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintAssistTypes.h"

class BLUEPRINTASSIST_API FBAInputProcessorState
{
public:
	// TOptional<FEdGraphPinType> CopiedPinType;

	bool OnKeyOrMouseDown(const FKey& Key);
	bool OnKeyOrMouseUp(const FKey& Key);

	bool TryCopyPastePinValue();

	bool TryFocusInDetailPanel();

	bool bConsumeMouseUp = false;

	// bool TryCopyPastePinType();
	// bool SpecialCopyPasteNode(); 
};
