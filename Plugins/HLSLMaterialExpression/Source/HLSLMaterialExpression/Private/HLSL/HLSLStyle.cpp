// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#include "HLSL/HLSLStyle.h"
#include "HLSL/Configuration.h"

#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"

#include "HLSLMaterialExpression.h"


TSharedPtr< FSlateStyleSet > FHLSLCodePaletteStyle::StyleSet = nullptr;
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

void FHLSLCodePaletteStyle::Initialize()
{
    if(StyleSet.IsValid())
    {
        return;
    }
    StyleSet = MakeShareable(new FSlateStyleSet("HLSLCodePalette"));
    StyleSet->SetContentRoot(FHLSLMaterialExpressionModule::HME_PluginPath/TEXT("Resources"));

    HLSLConfig::LoadConfig();

    const FSlateFontInfo RegularFont = FCoreStyle::GetDefaultFontStyle("Regular", 9);

    const FTextBlockStyle NormalText =
        FTextBlockStyle()
        .SetFont(RegularFont)
        .SetColorAndOpacity(FLinearColor::White)
        .SetShadowOffset(FVector2D::ZeroVector)
        .SetShadowColorAndOpacity(FLinearColor::White)
        .SetSelectedBackgroundColor(FLinearColor(FColor::FromHex(TEXT("00319AFF"))))
        .SetHighlightColor(FLinearColor(0.35f, 0.35f, 0.35f, 2.f))
        .SetHighlightShape(BOX_BRUSH("TextBlockHighlightShape", FMargin(3.f/8.f)));

    // Text editor
    {
        StyleSet->Set("HLSLPalette.NormalText", NormalText);

        StyleSet->Set("SyntaxHighlight.HLSL.Normal", FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->NormalColor)));
        StyleSet->Set("SyntaxHighlight.HLSL.Operator", FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->OperatorsColor)));
        StyleSet->Set("SyntaxHighlight.HLSL.Keyword", FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->KeywordsColor)));
        StyleSet->Set("SyntaxHighlight.HLSL.String", FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->StringsColor))); // pinkish
        StyleSet->Set("SyntaxHighlight.HLSL.Number", FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->NumberColor)));
        StyleSet->Set("SyntaxHighlight.HLSL.Comment", FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->CommentColor)));
        StyleSet->Set("SyntaxHighlight.HLSL.PreProcessorKeyword", FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->PreProcessorsColor)));
        StyleSet->Set("SyntaxHighlight.HLSL.Function", FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->FunctionsColor)));
        StyleSet->Set("SyntaxHighlight.HLSL.Struct", FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->StructsColor)));
        StyleSet->Set("SyntaxHighlight.HLSL.Define", FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->DefinesColor)));

        StyleSet->Set("HLSLPalette.Border", new BOX_BRUSH("Border", FMargin(4.0f/16.0f), FLinearColor(0.02f, 0.02f, 0.02f, 1)));

        const FButtonStyle Export = FButtonStyle()
            .SetNormal(BOX_BRUSH("Export", FVector2D(16, 16), 0.f))
            .SetHovered(BOX_BRUSH("Export", FVector2D(16, 16), 0.f, FLinearColor(0.75, 0.75, 0.75, 1)))
            .SetPressed(BOX_BRUSH("Export", FVector2D(16, 16), 0.f, FLinearColor::Gray))
            .SetNormalPadding(FMargin(0))
            .SetPressedPadding(FMargin(0));
        StyleSet->Set("HLSLPalette.Export", Export);

        const FButtonStyle Save = FButtonStyle()
            .SetNormal(BOX_BRUSH("Save", FVector2D(16, 16), 0.f))
            .SetHovered(BOX_BRUSH("Save", FVector2D(16, 16), 0.f, FLinearColor(0.75, 0.75, 0.75, 1)))
            .SetPressed(BOX_BRUSH("Save", FVector2D(16, 16), 0.f, FLinearColor::Gray))
            .SetNormalPadding(FMargin(0))
            .SetPressedPadding(FMargin(0));
        StyleSet->Set("HLSLPalette.Save", Save);

        const FButtonStyle Import = FButtonStyle()
            .SetNormal(BOX_BRUSH("Import", FVector2D(16, 16), 0.f))
            .SetHovered(BOX_BRUSH("Import", FVector2D(16, 16), 0.f, FLinearColor(0.75, 0.75, 0.75, 1)))
            .SetPressed(BOX_BRUSH("Import", FVector2D(16, 16), 0.f, FLinearColor::Gray))
            .SetNormalPadding(FMargin(0))
            .SetPressedPadding(FMargin(0));
        StyleSet->Set("HLSLPalette.Import", Import);

        const FButtonStyle Left = FButtonStyle()
            .SetNormal(BOX_BRUSH("Left", FVector2D(12, 12), 0.f))
            .SetHovered(BOX_BRUSH("Left", FVector2D(12, 12), 0.f, FLinearColor(0.75, 0.75, 0.75, 1)))
            .SetPressed(BOX_BRUSH("Left", FVector2D(12, 12), 0.f, FLinearColor::Gray))
            .SetNormalPadding(FMargin(0))
            .SetPressedPadding(FMargin(0));
        StyleSet->Set("HLSLPalette.Left", Left);

        const FButtonStyle Right = FButtonStyle()
            .SetNormal(BOX_BRUSH("Right", FVector2D(12, 12), 0.f))
            .SetHovered(BOX_BRUSH("Right", FVector2D(12, 12), 0.f, FLinearColor(0.75, 0.75, 0.75, 1)))
            .SetPressed(BOX_BRUSH("Right", FVector2D(12, 12), 0.f, FLinearColor::Gray))
            .SetNormalPadding(FMargin(0))
            .SetPressedPadding(FMargin(0));
        StyleSet->Set("HLSLPalette.Right", Right);

        const FButtonStyle Close = FButtonStyle()
            .SetNormal(BOX_BRUSH("Close", FVector2D(12, 12), 0.f))
            .SetHovered(BOX_BRUSH("Close", FVector2D(12, 12), 0.f, FLinearColor(0.75, 0.75, 0.75, 1)))
            .SetPressed(BOX_BRUSH("Close", FVector2D(12, 12), 0.f, FLinearColor::Gray))
            .SetNormalPadding(FMargin(0))
            .SetPressedPadding(FMargin(0));
        StyleSet->Set("HLSLPalette.Close", Close);


        StyleSet->Set("HLSLPalette.Settings", new IMAGE_BRUSH("Settings", FVector2D(16, 16)));

//         const FButtonStyle Settings = FButtonStyle()
//             .SetNormal(BOX_BRUSH("Settings", FVector2D(16, 16), 0.f))
//             .SetHovered(BOX_BRUSH("Settings", FVector2D(16, 16), 0.f, FLinearColor(0.75, 0.75, 0.75, 1)))
//             .SetPressed(BOX_BRUSH("Settings", FVector2D(16, 16), 0.f, FLinearColor::Gray))
//             .SetNormalPadding(FMargin(0))
//             .SetPressedPadding(FMargin(0));
//         StyleSet->Set("HLSLPalette.Settings", Settings);


        const FButtonStyle TextButton = FButtonStyle()
            .SetNormal(BOX_BRUSH("Border", FVector2D(16, 16), FMargin(4.0f/16.0f), FLinearColor(0.5f, 0.5f, 0.5f, 0.3f)))
            .SetHovered(BOX_BRUSH("Border", FVector2D(16, 16), FMargin(4.0f/16.0f), FLinearColor(0.45f, 0.5f, 0.3f, 0.6f)))
            .SetPressed(BOX_BRUSH("Border", FVector2D(16, 16), FMargin(4.0f/16.0f), FLinearColor(0.45f, 0.5f, 0.3f, 0.2f)))
            .SetNormalPadding(FMargin(0))
            .SetPressedPadding(FMargin(0));
        StyleSet->Set("HLSLPalette.TextButton", TextButton);

        const FEditableTextBoxStyle EditableTextBoxStyle = FEditableTextBoxStyle()
            .SetBackgroundImageNormal(FSlateNoResource())
            .SetBackgroundImageHovered(FSlateNoResource())
            .SetBackgroundImageFocused(FSlateNoResource())
            .SetBackgroundImageReadOnly(FSlateNoResource());

        StyleSet->Set("HLSLPalette.EditableTextBox", EditableTextBoxStyle);
    }

    FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
}

void FHLSLCodePaletteStyle::UpdateColors()
{
    {
        const FTextBlockStyle* Style = &FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Normal");
        const_cast<FTextBlockStyle*>(Style)->SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->NormalColor));
    }

    {
        const FTextBlockStyle* Style = &FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Operator");
        const_cast<FTextBlockStyle*>(Style)->SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->OperatorsColor));
    }

    {
        const FTextBlockStyle* Style = &FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Keyword");
        const_cast<FTextBlockStyle*>(Style)->SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->KeywordsColor));
    }


    {
        const FTextBlockStyle* Style = &FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.String");
        const_cast<FTextBlockStyle*>(Style)->SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->StringsColor));
    }

    {
        const FTextBlockStyle* Style = &FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Number");
        const_cast<FTextBlockStyle*>(Style)->SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->NumberColor));
    }

    {
        const FTextBlockStyle* Style = &FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Comment");
        const_cast<FTextBlockStyle*>(Style)->SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->CommentColor));
    }

    {
        const FTextBlockStyle* Style = &FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.PreProcessorKeyword");
        const_cast<FTextBlockStyle*>(Style)->SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->PreProcessorsColor));
    }

    {
        const FTextBlockStyle* Style = &FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Function");
        const_cast<FTextBlockStyle*>(Style)->SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->FunctionsColor));
    }

    {
        const FTextBlockStyle* Style = &FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Struct");
        const_cast<FTextBlockStyle*>(Style)->SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->StructsColor));
    }

    {
        const FTextBlockStyle* Style = &FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Define");
        const_cast<FTextBlockStyle*>(Style)->SetColorAndOpacity(FLinearColor(HLSLConfig::GetConfiguration()->DefinesColor));
    }
}

#undef BOX_BRUSH

void FHLSLCodePaletteStyle::Shutdown()
{
    if(StyleSet.IsValid())
    {
        FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
        ensure(StyleSet.IsUnique());
        StyleSet.Reset();
    }
}

const ISlateStyle& FHLSLCodePaletteStyle::Get()
{
    return *(StyleSet.Get());
}

const FName& FHLSLCodePaletteStyle::GetStyleSetName()
{
    return StyleSet->GetStyleSetName();
}