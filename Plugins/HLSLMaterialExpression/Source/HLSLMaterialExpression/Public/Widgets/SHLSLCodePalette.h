// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Framework/SlateDelegates.h"
#include "HLSLUtils.h"

class SHLSLCodePalette : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SHLSLCodePalette) 
    : _GetAdditionTokens(HLSLSyntax::FGetAdditionTokens())
    , _OnCodeSaved(FOnTextChanged())
    , _OnCodeChanged(FOnTextChanged())
    , _OnCodeCommitted(FOnTextCommitted()){}
    SLATE_EVENT(HLSLSyntax::FGetAdditionTokens, GetAdditionTokens)
    SLATE_EVENT(FOnTextChanged, OnCodeImported)
    SLATE_EVENT(FOnTextChanged, OnCodeSaved)
    SLATE_EVENT(FOnTextChanged, OnCodeChanged)
    SLATE_EVENT(FOnTextCommitted, OnCodeCommitted)
    SLATE_END_ARGS()

    ~SHLSLCodePalette();
    void Construct(const FArguments& InArgs, const FString& Code, bool AdvanceDisplay, 
    TMap<HLSLSyntax::ETokenType, TArray<FString>> AdditionTokens = TMap<HLSLSyntax::ETokenType, TArray<FString>>{});

    FString GetCode();
    void SetCode(const FString& Code);
    void UpdateCodeTokens(const TMap<HLSLSyntax::ETokenType,TArray<FString>>& AdditionTokens);

    void GotoLineAndColumn(int32 LineNumber, int32 ColumnNumber);
protected:
    virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
    virtual FReply OnKeyDown( const FGeometry& MyGeometry, const FKeyEvent& InKeyboardEvent ) override;
protected:
    FString SearchValue;
    FReply OnSearchKeyDown( const FGeometry& MyGeometry, const FKeyEvent& InKeyboardEvent );
    void OnSearchTextChanged(const FText& Text);
    void OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType);
    FReply SearchedPrevious();
    FReply SearchedNext();
    FReply CloseSearchBox();
    void ClearSearchResult();
public:
    FReply ExportAction();
    FReply ImportAction();
    FReply StyleSetupAction();
private:
    FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
protected:
    TSharedPtr<SScrollBar> HorizontalScrollbar;
    TSharedPtr<SScrollBar> VerticalScrollbar;
    TSharedPtr<class SHLSLCodeEditableText> CodeEditableText;
    TSharedPtr<class SHLSLTokenFinder> TokenFinder;

    SOverlay::FOverlaySlot* CodeSlot = nullptr;

    FOnTextChanged OnCodeImported;
private:
    static TArray<TWeakPtr<SHLSLCodePalette>> HLSLCodePalettes;
public:    
    static void UpdateCodeStyle();
};