// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "Input/Reply.h"
#include "HLSLUtils.h"


class SHLSLCodeEditableText : public SMultiLineEditableText
{
    SLATE_BEGIN_ARGS(SHLSLCodeEditableText) 
    : _GetAdditionTokens(HLSLSyntax::FGetAdditionTokens())
    , _OnCodeSaved(FOnTextChanged())
    , _OnCodeChanged(FOnTextChanged())
    , _OnCodeCommitted(FOnTextCommitted()){}
    SLATE_ATTRIBUTE(FText, Text)
    SLATE_ARGUMENT(TSharedPtr< SScrollBar >, HScrollBar)
    SLATE_ARGUMENT(TSharedPtr< SScrollBar >, VScrollBar)
    SLATE_EVENT(HLSLSyntax::FGetAdditionTokens, GetAdditionTokens)
    SLATE_EVENT(FOnTextChanged, OnCodeSaved)
    SLATE_EVENT(FOnTextChanged, OnCodeChanged)
    SLATE_EVENT(FOnTextCommitted, OnCodeCommitted)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, const TArray<HLSLSyntax::FRule>& AdditionTokens, bool AdvanceDisplay);
    void RegisterTextLayout(class FTextLayout* InTextLayout);
    void EnterSearchMode();
    void ExitSearchMode();
    void RemoveSearchTokenHighlight();
    void UpdateSearchTokenHighlight();
    void SetAdditionTokens(const TArray<HLSLSyntax::FRule>& NewTokens);
    TSharedPtr<SHLSLCodeEditableText>Duplicate(bool AdvanceDisplay);
public:
    static void ResetDefaultHLSLTokens();
    static TArray<HLSLSyntax::FRule>MakeTokens(const TMap<HLSLSyntax::ETokenType, TArray<FString>>& Tokens);
private:
    FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply OnKeyDown( const FGeometry& MyGeometry, const FKeyEvent& InKeyboardEvent ) override;
    virtual FReply OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent) override;
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
    void ClearComposingState();
private:
    HLSLSyntax::FGetAdditionTokens GetAdditionTokens;

    FOnTextChanged OnCodeSavedCallback = FOnTextChanged();

    TSharedPtr<class FHLSLTextLayoutMarshaller> HLSLRichTextMarshaller = nullptr;

    class FTextLayout* TextLayout = nullptr;
    TSharedPtr<class FHLSLTokenHighlighter> HLSLTokenHighlighter = nullptr;
};