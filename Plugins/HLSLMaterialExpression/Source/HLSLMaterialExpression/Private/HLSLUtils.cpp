// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#include "HLSLUtils.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "Framework/Application/SlateApplication.h"

DEFINE_LOG_CATEGORY(LogHLSLExpression);

#if WITH_EDITOR
class SSimpleMessages : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SSimpleMessages) {}
    SLATE_END_ARGS()
public:
    void Construct(const FArguments& InArgs, const TArray<FString>& Messages)
    {
        TSharedRef<SVerticalBox> List = SNew(SVerticalBox);
        ChildSlot
        [
            List
        ];

        auto AddMessage = [List] (const FString& Message)
        {
            List->AddSlot()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Top)
            .AutoHeight()
            .Padding(8.f,6.f)
            [
                SNew(SMultiLineEditableText)
                .AutoWrapText(false)
                .IsReadOnly(true)
                .Text(FText::FromString(Message))
            ];
        };

        for (const FString& Message : Messages)
        {
            AddMessage(Message);
        }
    }

    static void PopupMessages(const TArray<FString>& Messages)
    {
        TSharedRef<SWindow> NewWindow =
        SNew(SWindow)
        .Title(FText::FromString(TEXT("Compile Errors")))
        .SizingRule(ESizingRule::Autosized)
        .AutoCenter(EAutoCenter::PreferredWorkArea)
        .SupportsMinimize(false)
        .SupportsMaximize(false)
        .HasCloseButton(true)
        .bDragAnywhere(true)
        .IsTopmostWindow(true)
        .IsInitiallyMaximized(false)
        [
            SNew(SSimpleMessages,Messages)
        ];

        FSlateApplication::Get().AddWindow(NewWindow, true);
    }
};
#endif

void HLSLUtils::HLSLCompileError(const FString& Message,TArray<FString> Details)
{
#if WITH_EDITOR
    FNotificationInfo Info(FText::FromString(Message));
    Info.ExpireDuration = 10.f;
    Info.CheckBoxState = ECheckBoxState::Unchecked;

    if(Details.Num() > 0)
    {
        Info.HyperlinkText = FText::FromString(TEXT("Show Details"));
        Info.Hyperlink = FSimpleDelegate::CreateLambda([Details]()
        {
            SSimpleMessages::PopupMessages(Details);
        });
    }

    FSlateNotificationManager::Get().AddNotification(Info);
#endif
    UE_LOG(LogHLSLExpression, Error, TEXT("%s"), *Message);
}
