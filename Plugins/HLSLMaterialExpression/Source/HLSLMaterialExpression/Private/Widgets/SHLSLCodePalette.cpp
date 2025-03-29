// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#include "Widgets/SHLSLCodePalette.h"
#include "Widgets/SHLSLCodeEditableText.h"
#include "HLSL/HLSLStyle.h"
#include "HLSL/Configuration.h"
#include "Widgets/SHLSLStyleSetup.h"
#include "Widgets/SHLSLTokenFinder.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Misc/FileHelper.h"
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 2
#include "DesktopPlatformModule.h"
#else
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#endif

TArray<TWeakPtr<SHLSLCodePalette>> SHLSLCodePalette::HLSLCodePalettes = TArray<TWeakPtr<SHLSLCodePalette>>{};
void SHLSLCodePalette::UpdateCodeStyle()
{
    for (TWeakPtr<SHLSLCodePalette>Code : HLSLCodePalettes)
    {
        if(!Code.IsValid())
            continue;

        Code.Pin()->CodeEditableText = Code.Pin()->CodeEditableText->Duplicate(HLSLConfig::GetConfiguration()->bShowColors);
        Code.Pin()->CodeSlot->AttachWidget(Code.Pin()->CodeEditableText.ToSharedRef());
    }
}

SHLSLCodePalette::~SHLSLCodePalette()
{
    for ( auto It = HLSLCodePalettes.CreateIterator(); It; ++It )
    {
        if(!It->IsValid())
            It.RemoveCurrent();
    }
}

void SHLSLCodePalette::Construct(const FArguments& InArgs, const FString& Code, bool AdvanceDisplay, TMap<HLSLSyntax::ETokenType, TArray<FString>> AdditionTokens)
{
    OnCodeImported = InArgs._OnCodeImported;

    HorizontalScrollbar =
	SNew(SScrollBar)
	.Orientation(Orient_Horizontal)
	.Thickness(FVector2D(10.0f, 10.0f));

	VerticalScrollbar = 
    SNew(SScrollBar)
    .Orientation(Orient_Vertical)
    .Thickness(FVector2D(10.0f, 10.0f));

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FHLSLCodePaletteStyle::Get().GetBrush("HLSLPalette.Border"))
		[
			SNew(SGridPanel)
			.FillColumn(0, 1.0f)
			.FillRow(0, 1.0f)
			+SGridPanel::Slot(0, 0)
			[
				SNew(SOverlay)
                +SOverlay::Slot()
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Fill)
                .Padding(0.f)
                .Expose(CodeSlot)
                [
                    SAssignNew(CodeEditableText, SHLSLCodeEditableText, SHLSLCodeEditableText::MakeTokens(AdditionTokens), AdvanceDisplay)
				    .HScrollBar(HorizontalScrollbar)
				    .VScrollBar(VerticalScrollbar)
                    .GetAdditionTokens(InArgs._GetAdditionTokens)
                    .OnCodeSaved(InArgs._OnCodeSaved)
                    .OnCodeChanged(InArgs._OnCodeChanged)
                    .OnCodeCommitted(InArgs._OnCodeCommitted)
                ]
                +SOverlay::Slot()
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Top)
                .Padding(4.f)
                [
                    SAssignNew(TokenFinder, SHLSLTokenFinder)
                    .OnSearchKeyDown(this, &SHLSLCodePalette::OnSearchKeyDown)
                    .OnSearchChanged(this, &SHLSLCodePalette::OnSearchTextChanged)
                    .OnSearchCommitted(this, &SHLSLCodePalette::OnSearchTextCommitted)
                    .SearchedPrevious(this, &SHLSLCodePalette::SearchedPrevious)
                    .SearchedNext(this, &SHLSLCodePalette::SearchedNext)
                    .CloseFinder(this, &SHLSLCodePalette::CloseSearchBox)
                    .Visibility(EVisibility::Collapsed)
                ]
			]
			+SGridPanel::Slot(1, 0)
			[
				VerticalScrollbar.ToSharedRef()
			]
			+SGridPanel::Slot(0, 1)
			[
				HorizontalScrollbar.ToSharedRef()
			]
		]
	];

    CodeEditableText->RegisterTextLayout(nullptr);
    CodeEditableText->SetSelectWordOnMouseDoubleClick(true);

    SetCode(Code);
    HLSLCodePalettes.Add(TSharedPtr<SHLSLCodePalette>(StaticCastSharedRef<SHLSLCodePalette>(this->AsShared())));
}

FString SHLSLCodePalette::GetCode()
{
    return CodeEditableText->GetText().ToString();
}

void SHLSLCodePalette::SetCode(const FString& Code)
{
    CodeEditableText->SetText(FText::FromString(Code));
}

void SHLSLCodePalette::UpdateCodeTokens(const TMap<HLSLSyntax::ETokenType,TArray<FString>>& AdditionTokens)
{
    CodeEditableText->SetAdditionTokens(SHLSLCodeEditableText::MakeTokens(AdditionTokens));
}
 
void SHLSLCodePalette::GotoLineAndColumn(int32 LineNumber, int32 ColumnNumber)
{
    FTextLocation Location(LineNumber, ColumnNumber);
    CodeEditableText->GoTo(Location);
    CodeEditableText->ScrollTo(Location);
}

FReply SHLSLCodePalette::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    return FReply::Handled();
}

FReply SHLSLCodePalette::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyboardEvent)
{
    const FKey Key = InKeyboardEvent.GetKey();

    if(Key==EKeys::F && InKeyboardEvent.IsControlDown())
    {
        CodeEditableText->EnterSearchMode();
        TokenFinder->ShowFinder();
        return FReply::Handled();
    }
    return FReply::Handled();
}

FReply SHLSLCodePalette::OnSearchKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyboardEvent)
{
    const FKey Key = InKeyboardEvent.GetKey();

//     if(Key==EKeys::Enter&&!SearchValue.IsEmpty())
//     {
//         SearchedNext();
//     }

    return FReply::Unhandled();
}

void SHLSLCodePalette::OnSearchTextChanged(const FText& Text)
{
    if(Text.IsEmpty())
    {
        ClearSearchResult();
    }
    else if(!SearchValue.IsEmpty())
        CodeEditableText->UpdateSearchTokenHighlight();
}

void SHLSLCodePalette::OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
    if(SearchValue!=Text.ToString())
    {
        SearchValue = Text.ToString();
        CodeEditableText->SetSearchText(Text);
        CodeEditableText->UpdateSearchTokenHighlight();
    }
    else if(CommitType==ETextCommit::Type::OnEnter)
    {
        if(!SearchValue.IsEmpty())
        {
            SearchedNext();
        }
    }
}

FReply SHLSLCodePalette::SearchedPrevious()
{
    if(!SearchValue.IsEmpty())
    {
        CodeEditableText->AdvanceSearch(true);
        CodeEditableText->UpdateSearchTokenHighlight();
    }
    return FReply::Handled();
}

FReply SHLSLCodePalette::SearchedNext()
{
    if(!SearchValue.IsEmpty())
    {
        CodeEditableText->AdvanceSearch(false);
        CodeEditableText->UpdateSearchTokenHighlight();
    }
    return FReply::Handled();
}

FReply SHLSLCodePalette::CloseSearchBox()
{
    TokenFinder->CloseFinder();
    CodeEditableText->ExitSearchMode();
    ClearSearchResult();
    return FReply::Handled();
}

void SHLSLCodePalette::ClearSearchResult()
{
    SearchValue = TEXT("");
    CodeEditableText->ClearSelection();
    CodeEditableText->SetSearchText(FText());
    CodeEditableText->RemoveSearchTokenHighlight();
}

FReply SHLSLCodePalette::ExportAction()
{
    FString Code = CodeEditableText->GetText().ToString();
    if(Code.IsEmpty())
        return FReply::Handled();

    FString FileTypes = TEXT("Shader file .usf|*.usf|Shader file .ush|*.ush|Shader file .txt|*.txt");

    TArray<FString>SaveFilenames;
    const FString DefaultDirectory = FPaths::ProjectDir();
    const bool bSaved = FDesktopPlatformModule::Get()->SaveFileDialog
    (
        FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
        TEXT("Save HLSL Expression"),
        DefaultDirectory,
        TEXT("HLSLExpression"),
        *FileTypes,
        EFileDialogFlags::None,
        SaveFilenames
    );

    if(bSaved&&SaveFilenames.Num())
    {
        FFileHelper::SaveStringToFile(Code, *(SaveFilenames[0]), FFileHelper::EEncodingOptions::ForceUTF8);
    }
    return FReply::Handled();
}

FReply SHLSLCodePalette::ImportAction()
{
    TArray<FString>OpenFilenames;

    FString FileTypes = TEXT("Shader file .usf|*.usf|Shader file .ush|*.ush|Shader file .txt|*.txt");
    const FString DefaultDirectory = FPaths::ProjectDir();
    const bool bOpened = FDesktopPlatformModule::Get()->OpenFileDialog
    (
        FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
        TEXT("Import Code"),
        DefaultDirectory,
        TEXT(""),
        *FileTypes,
        EFileDialogFlags::Multiple,
        OpenFilenames
    );

    if(bOpened&&OpenFilenames.Num()>0)
    {
        FString TotalCode;
        for (FString& File : OpenFilenames)
        {
            FString Code;
            FString FullPath = FPaths::ConvertRelativePathToFull(File);
            FFileHelper::LoadFileToString(Code, *FullPath);
            TotalCode += FString::Printf(TEXT("\r\n//Import From %s.\r\n%s\r\n//Import End.\r\n\r\n"),*FullPath,*Code);
        }
        SetCode(TotalCode);

        OnCodeImported.ExecuteIfBound(CodeEditableText->GetText());
    }
    return FReply::Handled();
}

FReply SHLSLCodePalette::StyleSetupAction()
{
    SHLSLStyleSetup::ShowSetup(FVector2D(500,300));
    return FReply::Handled();
}

FReply SHLSLCodePalette::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
    return FReply::Unhandled();
}