// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#include "Widgets/SHLSLCodeViewer.h"
#include "Widgets/SHLSLCodePalette.h"
#include "HLSL/Configuration.h"

static TWeakPtr<SHLSLCodeViewer> Singleton = nullptr;
void SHLSLCodeViewer::PopupCodeViewer(class UMaterial* NewMaterial, FVector2D Position)
{
    if(!Singleton.IsValid())
    {
        TSharedRef<SWindow> NewWindow =
        SNew(SWindow)
        .Title(FText::FromString(TEXT("HLSL Editor")))
        .ClientSize(HLSLConfig::GetConfiguration()->HLSLViewer.ViewerSize)
        .SizingRule(ESizingRule::UserSized)
        .ScreenPosition(FVector2D(Position.X,Position.Y-HLSLConfig::GetConfiguration()->HLSLViewer.ViewerSize.Y*0.5f))
        .AutoCenter(EAutoCenter::None)
        .HasCloseButton(true)
        .SupportsMaximize(true)
        .SupportsMinimize(true)
        .bDragAnywhere(true)
        .IsTopmostWindow(true)
        .IsInitiallyMaximized(HLSLConfig::GetConfiguration()->HLSLViewer.bMaximize)
        [
            SAssignNew(Singleton, SHLSLCodeViewer)
        ];

        Singleton.Pin()->CodeViewerWindow = NewWindow;
        NewWindow->SetWidgetToFocusOnActivate(Singleton.Pin());
        FSlateApplication::Get().AddWindow(NewWindow, true);
    }

    Singleton.Pin()->SetMaterial(NewMaterial);
}

void SHLSLCodeViewer::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SBox)
        .MinDesiredWidth(400.f)
        .MinDesiredHeight(500.f)
        .Clipping(EWidgetClipping::ClipToBoundsAlways)
        [
            SAssignNew(CodePalette,SHLSLCodePalette,TEXT(""),HLSLConfig::GetConfiguration()->bShowColors)
        ]
    ];

    CachedCodeViewerSize = HLSLConfig::GetConfiguration()->HLSLViewer.ViewerSize;
    bMaximize = HLSLConfig::GetConfiguration()->HLSLViewer.bMaximize;
    SetCanTick(true);
}

void SHLSLCodeViewer::SetMaterial(class UMaterial* NewMaterial)
{ 
    if(!NewMaterial)
        return;

    FString Code;
    NewMaterial->GetMaterialResource(GMaxRHIFeatureLevel)->GetMaterialExpressionSource(Code);
    CodePalette->SetCode(Code);
}

void SHLSLCodeViewer::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    if(Singleton.IsValid()&&Singleton.Pin()->CodeViewerWindow.IsValid())
    {
        bool bDirty = false;

        if(Singleton.Pin()->CodeViewerWindow.Pin()->IsWindowMaximized()!=bMaximize)
        {
            bMaximize = !bMaximize;
            HLSLConfig::GetConfiguration()->HLSLViewer.bMaximize = bMaximize;
            bDirty = true;
        }

        if(!bMaximize)
        {
            if(!AllottedGeometry.GetLocalSize().Equals(CachedCodeViewerSize, 5.f))
            {
                CachedCodeViewerSize = AllottedGeometry.GetLocalSize();
                HLSLConfig::GetConfiguration()->HLSLViewer.ViewerSize = CachedCodeViewerSize;
                bDirty = true;
            }
        }

        if(bDirty)
            HLSLConfig::SaveStyles();
    }
}