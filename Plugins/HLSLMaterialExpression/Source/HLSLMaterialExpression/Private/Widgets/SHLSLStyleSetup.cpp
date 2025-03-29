// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#include "Widgets/SHLSLStyleSetup.h"
#include "Widgets/SHLSLCodeEditableText.h"
#include "Widgets/SHLSLCodePalette.h"
#include "HLSL/HLSLStyle.h"
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 2
#include "Widgets/Colors/SColorPicker.h"
#else
#include "AppFramework/Public/Widgets/Colors/SColorPicker.h"
#endif
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Framework/Application/SlateApplication.h"

static TWeakPtr<class SWindow> HLSLStyleSetupWindow;
void SHLSLStyleSetup::Construct(const FArguments& InArgs)
{
    TSharedRef<SVerticalBox> List = SNew(SVerticalBox);
    ChildSlot
    [
        SNew(SVerticalBox)
        +SVerticalBox::Slot()
        .VAlign(VAlign_Fill)
        .HAlign(HAlign_Fill)
        .FillHeight(1.0)
        [
            List
        ]
        +SVerticalBox::Slot()
        .VAlign(VAlign_Fill)
        .HAlign(HAlign_Center)
        .Padding(FMargin(0,16.f,0.f,0.f))
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            +SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            .HAlign(HAlign_Center)
            .Padding(FMargin(8.f,0.f))
            [
                SNew(SBox)
                .WidthOverride(96.f)
                .HeightOverride(24.f)
                [
                    SNew(SButton)
                    .ButtonStyle(&FHLSLCodePaletteStyle::Get().GetWidgetStyle<FButtonStyle>("HLSLPalette.TextButton"))
                    .OnClicked(this, &SHLSLStyleSetup::Apply)
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                        .ColorAndOpacity(FColor::White)
                        .Margin(0.f)
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
                        .Text(FText::FromString(TEXT("Apply")))
                    ]
                ]
            ]
            +SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            .HAlign(HAlign_Center)
            .Padding(FMargin(8.f,0.f))
            [
                SNew(SBox)
                .WidthOverride(128.f)
                .HeightOverride(24.f)
                [
                    SNew(SButton)
                    .ButtonStyle(&FHLSLCodePaletteStyle::Get().GetWidgetStyle<FButtonStyle>("HLSLPalette.TextButton"))
                    .OnClicked(this, &SHLSLStyleSetup::ResetToDefault)
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                        .ColorAndOpacity(FColor::White)
                        .Margin(0.f)
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
                        .Text(FText::FromString(TEXT("Reset to Default")))
                    ]
                ]
            ]
        ]
    ];

    auto AddSection =[List](const FString& Name, TSharedRef<SWidget> Section, bool bFillWidth)
    {
        List->AddSlot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Top)
        .AutoHeight()
        .Padding(8.f,6.f)
        [
            SNew(SBox)
            .HeightOverride(24.f)
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            .Padding(0.f)
            [
                SNew(SHorizontalBox)
                +SHorizontalBox::Slot()
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Center)
                .Padding(0.f,0.f,8.f,0.f)
                .FillWidth(0.4f)
                [
                    SNew(SBox)
                    .HAlign(HAlign_Left)
                    [
                        SNew(STextBlock)
                        .ColorAndOpacity( FColor::White )
                        .Margin(0.f)
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold",12))
                        .Text(FText::FromString(Name))
                    ]
                ]
                 +SHorizontalBox::Slot()
                 .HAlign(HAlign_Fill)
                 .VAlign(VAlign_Center)
                 .Padding(0.f, 0.f, 8.f, 0.f)
                 .FillWidth(0.6f)
                 [
                     SNew(SBox)
                    .HAlign(bFillWidth?HAlign_Fill:HAlign_Left)
                    [
                        Section
                    ]
                 ]
            ]
        ];
    };


    Config = *(HLSLConfig::GetConfiguration().Get());
    AddSection(TEXT("Show Colors"),
    SNew(SCheckBox)
    .OnCheckStateChanged_Lambda([this](const ECheckBoxState NewCheckedState)
    {
        Config.bShowColors = NewCheckedState==ECheckBoxState::Checked;
    })
    .IsChecked_Lambda([this](){if(Config.bShowColors)return ECheckBoxState::Checked; return ECheckBoxState::Unchecked;})
    .Cursor(EMouseCursor::Default),false);

    AddSection(TEXT("Normal Color"),
    ConstructColorBlock(Config.NormalColor,
    [this](){return Config.NormalColor;},
    [this](FLinearColor Color){Config.NormalColor = Color.ToFColor(true);}),true);

    AddSection(TEXT("Number Color"),
    ConstructColorBlock(Config.NumberColor,
    [this](){return Config.NumberColor;},
    [this](FLinearColor Color){Config.NumberColor = Color.ToFColor(true);}),true);

    AddSection(TEXT("Comment Color"),
    ConstructColorBlock(Config.CommentColor,
    [this](){return Config.CommentColor;},
    [this](FLinearColor Color){Config.CommentColor = Color.ToFColor(true);}),true);

    AddSection(TEXT("PreProcessors Color"),
    ConstructColorBlock(Config.PreProcessorsColor,
    [this](){return Config.PreProcessorsColor;},
    [this](FLinearColor Color){Config.PreProcessorsColor = Color.ToFColor(true);}),true);

    AddSection(TEXT("Operators Color"),
    ConstructColorBlock(Config.OperatorsColor,
    [this](){return Config.OperatorsColor;},
    [this](FLinearColor Color){Config.OperatorsColor = Color.ToFColor(true);}),true);

    AddSection(TEXT("Keywords Color"),
    ConstructColorBlock(Config.KeywordsColor,
    [this](){return Config.KeywordsColor;},
    [this](FLinearColor Color){Config.KeywordsColor = Color.ToFColor(true);}),true);

    AddSection(TEXT("Strings Color"),
    ConstructColorBlock(Config.StringsColor,
    [this](){return Config.StringsColor;},
    [this](FLinearColor Color){Config.StringsColor = Color.ToFColor(true);}),true);

    AddSection(TEXT("Functions Color"),
    ConstructColorBlock(Config.FunctionsColor,
    [this](){return Config.FunctionsColor;},
    [this](FLinearColor Color){Config.FunctionsColor = Color.ToFColor(true);}),true);

    AddSection(TEXT("Structs Color"),
    ConstructColorBlock(Config.StructsColor,
    [this](){return Config.StructsColor;},
    [this](FLinearColor Color){Config.StructsColor = Color.ToFColor(true);}),true);

    AddSection(TEXT("Defines Color"),
    ConstructColorBlock(Config.DefinesColor,
    [this](){return Config.DefinesColor;},
    [this](FLinearColor Color){Config.DefinesColor = Color.ToFColor(true);}),true);
}

FReply SHLSLStyleSetup::Apply()
{
    SHLSLCodeEditableText::ResetDefaultHLSLTokens();
    *(HLSLConfig::GetConfiguration().Get()) = Config;
    FHLSLCodePaletteStyle::UpdateColors();
    SHLSLCodePalette::UpdateCodeStyle();
    HLSLConfig::SaveStyles();
    return FReply::Handled();
}

FReply SHLSLStyleSetup::ResetToDefault()
{
    Config.CopyColors(HLSLConfig::GetDefaultConfiguration());
    return Apply();
}

void SHLSLStyleSetup::ShowSetup(const FVector2D& ScreenPosition)
{
    if(!HLSLStyleSetupWindow.IsValid())
    {
        FVector2D Size(400.f,450.f);
        TSharedRef<SWindow> NewWindow =
        SAssignNew(HLSLStyleSetupWindow,SWindow)
        .Title(FText::FromString(TEXT("HLSL Editor")))
        .ClientSize(Size)
        .SizingRule(ESizingRule::UserSized)
        .ScreenPosition(FVector2D(ScreenPosition.X,ScreenPosition.Y))
        .AutoCenter(EAutoCenter::None)
        .HasCloseButton(true)
        .SupportsMaximize(false)
        .SupportsMinimize(false)
        .bDragAnywhere(true)
        .IsTopmostWindow(true)
        .IsInitiallyMaximized(false)
        [
            SNew(SHLSLStyleSetup)
        ];

        FSlateApplication::Get().AddWindow(NewWindow, true);
    }
}

TSharedRef<class SColorBlock> SHLSLStyleSetup::ConstructColorBlock(const FLinearColor& DefaultColor,
TFunction<FLinearColor()>GetColor, TFunction<void(const FLinearColor)>SetColor)
{
    TWeakPtr<SColorBlock> WeakColorBlock;
    TSharedRef<class SColorBlock> ColorBlock = SAssignNew(WeakColorBlock,SColorBlock);

    SColorBlock::FArguments Arguments;
    Arguments.Color_Lambda([GetColor](){return GetColor();});
    Arguments.ShowBackgroundForAlpha(false);
#if ENGINE_MAJOR_VERSION == 4 
    Arguments.IgnoreAlpha(true);
#else
    Arguments.AlphaDisplayMode(EColorBlockAlphaDisplayMode::Ignore);
#endif
    Arguments.Size(FVector2D(35.0f, 12.0f));
    Arguments.OnMouseButtonDown
    (
        FPointerEventHandler::CreateLambda([WeakColorBlock, DefaultColor, GetColor, SetColor] (const FGeometry&, const FPointerEvent&)
        {
            if(!WeakColorBlock.IsValid())
                return FReply::Handled();

            FColorPickerArgs PickerArgs;
            PickerArgs.bUseAlpha = false;
            PickerArgs.bOnlyRefreshOnMouseUp = false;
            PickerArgs.bOnlyRefreshOnOk = false;
            PickerArgs.sRGBOverride = true;
            PickerArgs.DisplayGamma = GEngine->GetDisplayGamma();
            PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateLambda([SetColor] (FLinearColor Color) { SetColor(Color); });
            PickerArgs.OnColorPickerCancelled = FOnColorPickerCancelled::CreateLambda([DefaultColor, SetColor] (FLinearColor Color) { SetColor(DefaultColor); });
            //PickerArgs.OnColorPickerWindowClosed = FOnWindowClosed::CreateLambda([DefaultColor, SetColor] (const TSharedRef<SWindow>&) { SetColor(DefaultColor); });
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 1
            PickerArgs.InitialColor = GetColor();
#else
            PickerArgs.InitialColorOverride = GetColor();
#endif
            PickerArgs.ParentWidget = WeakColorBlock.Pin();
            PickerArgs.OptionalOwningDetailsView = WeakColorBlock.Pin();
            FWidgetPath ParentWidgetPath;
            if(FSlateApplication::Get().FindPathToWidget(WeakColorBlock.Pin().ToSharedRef(), ParentWidgetPath))
            {
                PickerArgs.bOpenAsMenu = FSlateApplication::Get().FindMenuInWidgetPath(ParentWidgetPath).IsValid();
            }

            OpenColorPicker(PickerArgs);
            return FReply::Handled();
        })
    );
    ColorBlock->Construct(Arguments);

    return ColorBlock;
}