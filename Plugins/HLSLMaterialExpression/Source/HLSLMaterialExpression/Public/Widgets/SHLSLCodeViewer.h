// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Framework/SlateDelegates.h"


class SHLSLCodeViewer : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SHLSLCodeViewer) {}
    SLATE_END_ARGS()
public:
    static void PopupCodeViewer(class UMaterial* NewMaterial, FVector2D Position);
public:
    void Construct(const FArguments& InArgs);
protected:
    void SetMaterial(class UMaterial* NewMaterial);
protected:
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);
private:
    bool bMaximize = false;
    FVector2D CachedCodeViewerSize = FVector2D::ZeroVector;
    TSharedPtr<class SHLSLCodePalette> CodePalette;
    TWeakPtr<class SWindow> CodeViewerWindow = nullptr;
};