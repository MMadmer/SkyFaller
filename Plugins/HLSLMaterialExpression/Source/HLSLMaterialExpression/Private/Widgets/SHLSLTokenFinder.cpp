// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#include "Widgets/SHLSLTokenFinder.h"
#include "HLSL/HLSLStyle.h"
#include "Widgets/Input/SSearchBox.h"
#include "Framework/Application/SlateApplication.h"

void SHLSLTokenFinder::Construct(const FArguments& InArgs)
{
    this->ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
        .HAlign(HAlign_Right)
		.AutoHeight()
		[
            SAssignNew(SearchTextField, SSearchBox)
            .OnKeyDownHandler(InArgs._OnSearchKeyDown)
            .HintText(FText::FromString(TEXT("Search Token...")))
            .OnTextChanged(InArgs._OnSearchChanged)
            .OnTextCommitted(InArgs._OnSearchCommitted)
		]
        +SVerticalBox::Slot()
        .HAlign(HAlign_Right)
        .Padding(0,4.f,0.f,0.f)
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            +SHorizontalBox::Slot()
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Center)
            .Padding(4.f,2.f)
            .AutoWidth()
            [
                SNew(SButton)
                .ButtonStyle(&FHLSLCodePaletteStyle::Get().GetWidgetStyle<FButtonStyle>("HLSLPalette.Left"))
                .OnClicked(InArgs._SearchedPrevious)
            ]
            +SHorizontalBox::Slot()
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Center)
            .Padding(4.f,2.f)
            .AutoWidth()
            [
                SNew(SButton)
                .ButtonStyle(&FHLSLCodePaletteStyle::Get().GetWidgetStyle<FButtonStyle>("HLSLPalette.Right"))
                .OnClicked(InArgs._SearchedNext)
            ]
            +SHorizontalBox::Slot()
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Center)
            .Padding(4.f, 2.f)
            .AutoWidth()
            [
                SNew(SButton)
                .ButtonStyle(&FHLSLCodePaletteStyle::Get().GetWidgetStyle<FButtonStyle>("HLSLPalette.Close"))
                .OnClicked(InArgs._CloseFinder)
            ]
        ]
	];
}

void SHLSLTokenFinder::FocusForUse()
{
    FWidgetPath FilterTextBoxWidgetPath;
    FSlateApplication::Get().GeneratePathToWidgetUnchecked(SearchTextField.ToSharedRef(), FilterTextBoxWidgetPath);
    FSlateApplication::Get().SetKeyboardFocus(FilterTextBoxWidgetPath, EFocusCause::SetDirectly);
}

void SHLSLTokenFinder::ShowFinder()
{
    SetVisibility(EVisibility::Visible);
    FocusForUse();
}

void SHLSLTokenFinder::CloseFinder()
{
    SetVisibility(EVisibility::Collapsed);
}