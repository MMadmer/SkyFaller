// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Framework/SlateDelegates.h"

class SHLSLTokenFinder : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SHLSLTokenFinder) 
    : _OnSearchKeyDown(FOnKeyDown())
    , _OnSearchChanged(FOnTextChanged())
    , _OnSearchCommitted(FOnTextCommitted())
    , _SearchedPrevious(FOnClicked())
    , _SearchedNext(FOnClicked())
    , _CloseFinder(FOnClicked()){}
    SLATE_EVENT(FOnKeyDown, OnSearchKeyDown)
    SLATE_EVENT(FOnTextChanged, OnSearchChanged)
    SLATE_EVENT(FOnTextCommitted, OnSearchCommitted)
    SLATE_EVENT(FOnClicked, SearchedPrevious)
    SLATE_EVENT(FOnClicked, SearchedNext)
    SLATE_EVENT(FOnClicked, CloseFinder)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    void FocusForUse();
    void ShowFinder();
    void CloseFinder();
protected:
    TSharedPtr<class SSearchBox> SearchTextField;
};