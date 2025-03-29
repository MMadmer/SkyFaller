// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Framework/SlateDelegates.h"
#include "Input/Reply.h"
#include "HLSL/Configuration.h"

class SHLSLStyleSetup : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SHLSLStyleSetup) {}
    SLATE_END_ARGS()
public:
    void Construct(const FArguments& InArgs);
    FReply Apply();
    FReply ResetToDefault();
    static void ShowSetup(const FVector2D& ScreenPosition);
protected:
    TSharedRef<class SColorBlock> ConstructColorBlock(const FLinearColor& DefaultColor,TFunction<FLinearColor()>GetColor, TFunction<void(const FLinearColor)>SetColor);
private:
    HLSLConfig::FConfiguration Config;
};