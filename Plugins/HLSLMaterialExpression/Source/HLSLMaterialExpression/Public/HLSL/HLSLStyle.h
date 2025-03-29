// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FHLSLCodePaletteStyle
{
public:
    static void Initialize();
    static void UpdateColors();
    static void Shutdown();
    static const ISlateStyle& Get();
    static const FName& GetStyleSetName();
    static TSharedPtr< class FSlateStyleSet > StyleSet;
};