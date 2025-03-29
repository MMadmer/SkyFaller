// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#include "Graph/HLSLGraphNode.h"
#include "HLSL/TokenWatcher.h"
#include "HLSL/Expression.h"
#include "HLSL/Configuration.h"
#include "HLSL/HLSLParser.h"
#include "HLSL/HLSLStyle.h"
#include "HLSL/HLSLExporter.h"

#include "Widgets/SHLSLCodeEditableText.h"
#include "Widgets/SHLSLCodePalette.h"
#include "Widgets/SHLSLCodeViewer.h"

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 2
#include "MaterialGraph/MaterialGraphNode.h"
#else
#include "UnrealEd/Classes/MaterialGraph/MaterialGraphNode.h"
#endif
#include "SGraphPin.h"
#include "SGraphPanel.h"
#include "GraphEditorSettings.h"

#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SCheckBox.h"

#include "Styling/SlateTypes.h"


#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 2
#include "MaterialEditorUtilities.h"
#else
#include "MaterialEditor/Public/MaterialEditorUtilities.h"
#endif
#include "TutorialMetaData.h"

#include "Developer/ShaderCompilerCommon/Public/ShaderCompilerCommon.h"

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
#include "Styling/AppStyle.h"
#endif


FName GetInputPinSubCategory(EHLSLInputType Type)
{
    switch (Type)
    {
    case EHLSLInputType::HIPT_Float:
        return TEXT("float");
    case EHLSLInputType::HIPT_Float2:
        return TEXT("float2");
    case EHLSLInputType::HIPT_Float3:
        return TEXT("float3");
    case EHLSLInputType::HIPT_Float4:
        return TEXT("float4"); 
    case EHLSLInputType::HIPT_Texture2D:
    case EHLSLInputType::HIPT_Texture2DArray:
    case EHLSLInputType::HIPT_TextureExternal:
    case EHLSLInputType::HIPT_TextureCube:
    case EHLSLInputType::HIPT_VolumeTexture:
        return TEXT("object");
    case EHLSLInputType::HIPT_MaterialAttributes:
        return TEXT("struct");
    default:
        return TEXT("void");
    }
}

FName GetOutputPinSubCategory(EHLSLOutputType Type)
{
    switch (Type)
    {
	case EHLSLOutputType::HOPT_Float:
		return TEXT("float");
	case EHLSLOutputType::HOPT_Float2:
		return TEXT("float2");
	case EHLSLOutputType::HOPT_Float3:
		return TEXT("float3");
	case EHLSLOutputType::HOPT_Float4:
		return TEXT("float4");
	case EHLSLOutputType::HOPT_MaterialAttributes:
        return TEXT("struct");
    default:
        return TEXT("void");
    }
}

SHLSLGraphNode::~SHLSLGraphNode()
{
    TWeakObjectPtr<UMaterial> Material = HLSLGraphNode->MaterialExpression->Material;
    FHLSLTokenWatcher::GetTokenWatcher()->UnregisterCodePalette(Material, CodePalette);
    //     if(!Material->IsPendingKill()&&HLSLGraphNode->IsPendingKill())
    //     {
    //         if(FHLSLTokenWatcher::GetTokenWatcher()->CompareAndRecollectTokens(Material))
    //             FHLSLTokenWatcher::GetTokenWatcher()->UpdateCodePaletteTokens(Material);
    //     }

//     if (HLSLGraphNode->MaterialExpression->IsPendingKill())
//     {
// 		bool bUpdate = false;
// 		for (int32 PinIndex = 0; PinIndex < GraphNode->Pins.Num(); ++PinIndex)
// 		{
// 			UEdGraphPin* CurPin = GraphNode->Pins[PinIndex];
// 
// 			if (CurPin->HasAnyConnections())
// 			{
// 				bUpdate = true;
// 				CurPin->BreakAllPinLinks(true);
// 			}
// 		}
// 
// 		if (bUpdate)
// 		{
// 			FMaterialEditorUtilities::UpdateMaterialAfterGraphChange(HLSLGraphNode->GetGraph());
// 		}
//     }
}

void SHLSLGraphNode::Construct(const FArguments& InArgs, class UMaterialGraphNode* Node)
{
    HLSLGraphNode = Node;
    Cast<UHLSLMaterialExpression>(Node->MaterialExpression)->UpdateGraphCodeCallback.BindSP(this,&SHLSLGraphNode::ResetCode);

    GraphNode = Node;
    SetCursor(EMouseCursor::CardinalCross);
    ZoomCurve = SpawnAnim.AddCurve(0, 0.1f);
    FadeCurve = SpawnAnim.AddCurve(0.15f, 0.15f);
    UserSize.X = Node->NodeWidth;
    UserSize.Y = Node->NodeHeight;
    MouseZone = CRWZ_NotInWindow;
    bUserIsDragging = false;

    UpdateGraphNode();

    bDirty = false;
}

void SHLSLGraphNode::CreatePinWidgets()
{
    for(int32 PinIndex = 0; PinIndex<GraphNode->Pins.Num(); ++PinIndex)
    {
        UEdGraphPin* CurPin = GraphNode->Pins[PinIndex];

        TSharedPtr<SGraphPin> NewPin = CreatePinWidget(CurPin);
        check(NewPin.IsValid());

        this->AddPin(NewPin.ToSharedRef());
    }
}

FReply SHLSLGraphNode::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    FReply Reply = SGraphNodeResizable::OnMouseMove(MyGeometry, MouseEvent);
    if(bUserIsDragging&&HLSLGraphNode.IsValid())
    {
        if(UHLSLMaterialExpression* HLSL = Cast<UHLSLMaterialExpression>(HLSLGraphNode->MaterialExpression))
        {
            if(!HLSL->bCollapsed)
            {
                HLSL->SetCodePaletteSize(FIntPoint(UserSize.X, UserSize.Y));
            }
        }

        HLSLGraphNode->MaterialExpression->MarkPackageDirty();
        HLSLGraphNode->MaterialDirtyDelegate.ExecuteIfBound();
    }
    return Reply;
}

FReply SHLSLGraphNode::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
    FVector2D LocalPos = InMyGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
    if(LocalPos.Y>0&&LocalPos.Y<24&&LocalPos.X > 0&&LocalPos.X<GetDesiredSize().X)
    {
        if(HLSLGraphNode.IsValid())
            SHLSLCodeViewer::PopupCodeViewer(HLSLGraphNode->MaterialExpression->Material, InMouseEvent.GetScreenSpacePosition());
    }

    return FReply::Handled();
}

FReply SHLSLGraphNode::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if((MouseEvent.GetEffectingButton()==EKeys::LeftMouseButton)&&bUserIsDragging)
    {
        bUserIsDragging = false;

        // Resize the node	
        UserSize.X = FMath::RoundToFloat(UserSize.X);
        UserSize.Y = FMath::RoundToFloat(UserSize.Y);

        GetNodeObj()->ResizeNode(UserSize);

        ResizeTransactionPtr.Reset();

        return FReply::Handled().ReleaseMouseCapture();
    }
    return FReply::Unhandled();
}

void SHLSLGraphNode::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
    UHLSLMaterialExpression* HLSLExpression = Cast<UHLSLMaterialExpression>(HLSLGraphNode->MaterialExpression);
    if(HLSLExpression->Material->bEnableNewHLSLGenerator)
	{
		TArray<FHLSLInput>& Inputs = HLSLExpression->HLSLInputs;
		TArray<FHLSLOutput>& Outputs = HLSLExpression->HLSLOutputs;
		for (int32 PinIndex = 0; PinIndex < GraphNode->Pins.Num(); ++PinIndex)
		{
			UEdGraphPin* CurPin = GraphNode->Pins[PinIndex];
            bool bHasConnection = CurPin->HasAnyConnections();
			if (CurPin->PinName == HLSL_OUTPUT_PIXEL_NAME)
			{
                CurPin->PinType.PinCategory = TEXT("value");
				CurPin->PinType.PinSubCategory = TEXT("struct");
				break;
			}
            else if (CurPin->PinName == HLSL_OUTPUT_VERTEX_NAME)
            {
				CurPin->PinType.PinCategory = TEXT("value");
				CurPin->PinType.PinSubCategory = TEXT("float3");
            }
			else
			    CurPin->PinType.PinCategory = bHasConnection ? TEXT("value") : TEXT("void");

			if (bHasConnection)
			{
				if (CurPin->Direction == EEdGraphPinDirection::EGPD_Input)
				{
					int32 Index = 0;
					while (Index < Inputs.Num())
					{
						if (Inputs[Index].InputName == CurPin->PinName)
						{
							CurPin->PinType.PinSubCategory = GetInputPinSubCategory(Inputs[Index].InputType);
							break;
						}
						++Index;
					}
				}
				else
				{
					int32 Index = 0;
					while (Index < Outputs.Num())
					{
						if (Outputs[Index].OutputName == CurPin->PinName)
						{
							CurPin->PinType.PinSubCategory = GetOutputPinSubCategory(Outputs[Index].OutputType);
							break;
						}
						++Index;
					}
				}
			}
		}
	}
#endif
    SGraphNode::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

FReply SHLSLGraphNode::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    return FReply::Unhandled();
}

void SHLSLGraphNode::OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{

}

void SHLSLGraphNode::EndUserInteraction() const
{

}

FVector2D SHLSLGraphNode::ComputeDesiredSize(float Scale) const
{
    if(HLSLGraphNode.IsValid()&&HLSLGraphNode->MaterialExpression->bCollapsed)
    {
        return SGraphNode::ComputeDesiredSize(Scale);
    }
    return UserSize;
}

bool SHLSLGraphNode::IsNameReadOnly() const
{
    return SGraphNode::IsNameReadOnly();
}

#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
void SHLSLGraphNode::MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty)
#else
void SHLSLGraphNode::MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter)
#endif
{

#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
    SGraphNodeResizable::MoveTo(NewPosition, NodeFilter, bMarkDirty);
#else
    SGraphNodeResizable::MoveTo(NewPosition, NodeFilter);
#endif

    if(HLSLGraphNode.IsValid())
    {
        HLSLGraphNode->MaterialExpression->MaterialExpressionEditorX = HLSLGraphNode->NodePosX;
        HLSLGraphNode->MaterialExpression->MaterialExpressionEditorY = HLSLGraphNode->NodePosY;

        if(UHLSLMaterialExpression* HLSL = Cast<UHLSLMaterialExpression>(HLSLGraphNode->MaterialExpression))
        {
            HLSL->SetCodePalettePosition(FIntPoint(HLSLGraphNode->NodePosX, HLSLGraphNode->NodePosY));
        }

        HLSLGraphNode->MaterialExpression->MarkPackageDirty();
        HLSLGraphNode->MaterialDirtyDelegate.ExecuteIfBound();
    }
}

FSlateRect SHLSLGraphNode::GetHitTestingBorder() const
{
    return FSlateRect(8, 8, 8, 8);
}

FVector2D SHLSLGraphNode::GetNodeMaximumSize() const
{
    return FVector2D(1920, 1080);
}

void SHLSLGraphNode::UpdateGraphNode()
{
    InputPins.Empty();
    OutputPins.Empty();

    RightNodeBox.Reset();
    LeftNodeBox.Reset();

    TSharedPtr<SVerticalBox> MainVerticalBox;
    SetupErrorReporting();

    TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

    IconColor = FLinearColor::White;
    const FSlateBrush* IconBrush = nullptr;
    if(GraphNode!=NULL&&GraphNode->ShowPaletteIconOnNode())
    {
        IconBrush = GraphNode->GetIconAndTint(IconColor).GetOptionalIcon();
    }

    TSharedRef<SOverlay> DefaultTitleAreaWidget =
    SNew(SOverlay)
    +SOverlay::Slot()
    [
        SNew(SImage)
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
        .Image(FAppStyle::GetBrush("Graph.Node.TitleGloss"))
#else
        .Image(FEditorStyle::GetBrush("Graph.Node.TitleGloss"))
#endif
        .ColorAndOpacity(this, &SGraphNode::GetNodeTitleIconColor)
    ]
    +SOverlay::Slot()
    .HAlign(HAlign_Fill)
    .VAlign(VAlign_Center)
    [
        SNew(SHorizontalBox)
        +SHorizontalBox::Slot()
        .HAlign(HAlign_Fill)
        [
            SNew(SBorder)
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
            .BorderImage(FAppStyle::GetBrush("Graph.Node.ColorSpill"))
#else
            .BorderImage(FEditorStyle::GetBrush("Graph.Node.ColorSpill"))
#endif
            .Padding(FMargin(10, 5, 30, 3))
            .BorderBackgroundColor(this, &SGraphNode::GetNodeTitleColor)
            [
                SNew(SHorizontalBox)
                +SHorizontalBox::Slot()
                .VAlign(VAlign_Top)
                .Padding(FMargin(0.f, 0.f, 4.f, 0.f))
                .AutoWidth()
                [
                    SNew(SImage)
                    .Image(IconBrush)
                    .ColorAndOpacity(this, &SGraphNode::GetNodeTitleIconColor)
                ]
                +SHorizontalBox::Slot()
                [
                    SNew(SVerticalBox)
                    +SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        CreateTitleWidget(NodeTitle)
                    ]
                   +SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        NodeTitle.ToSharedRef()
                    ]
                ]
            ]
        ]
       +SHorizontalBox::Slot()
       .HAlign(HAlign_Right)
       .VAlign(VAlign_Center)
       .Padding(0, 0, 5, 0)
       .AutoWidth()
       [
            CreateTitleRightWidget()
       ]
    ]
    +SOverlay::Slot()
    .VAlign(VAlign_Top)
    [
        SNew(SBorder)
        .Visibility(EVisibility::HitTestInvisible)
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
        .BorderImage(FAppStyle::GetBrush("Graph.Node.TitleHighlight"))
#else
        .BorderImage(FEditorStyle::GetBrush("Graph.Node.TitleHighlight"))
#endif
        .BorderBackgroundColor(this, &SGraphNode::GetNodeTitleIconColor)
        [
            SNew(SSpacer)
            .Size(FVector2D(20, 20))
        ]
    ];

    SetDefaultTitleAreaWidget(DefaultTitleAreaWidget);

    FGraphNodeMetaData TagMeta(TEXT("HLSLGraphNode"));
    PopulateMetaTag(&TagMeta);

    TSharedPtr<SVerticalBox> InnerVerticalBox;
    this->ContentScale.Bind(this, &SGraphNode::GetContentScale);

    InnerVerticalBox = 
    SNew(SVerticalBox)
    +SVerticalBox::Slot()
    .AutoHeight()
    .HAlign(HAlign_Fill)
    .VAlign(VAlign_Top)
    .Padding(Settings->GetNonPinNodeBodyPadding())
    [
        DefaultTitleAreaWidget
    ]
    +SVerticalBox::Slot()
    .FillHeight(1.f)
    .HAlign(HAlign_Fill)
    .VAlign(VAlign_Fill)
    [
        CreateNodeContentArea()
    ];

    TSharedPtr<SWidget> EnabledStateWidget = GetEnabledStateWidget();
    if(EnabledStateWidget.IsValid())
    {
        InnerVerticalBox->AddSlot()
            .AutoHeight()
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Top)
            .Padding(FMargin(2, 0))
            [
                EnabledStateWidget.ToSharedRef()
            ];
    }

    InnerVerticalBox->AddSlot()
    .AutoHeight()
    .Padding(Settings->GetNonPinNodeBodyPadding())
    [
        ErrorReporting->AsWidget()
    ];

    this->GetOrAddSlot(ENodeZone::Center)
    .HAlign(HAlign_Fill)
    .VAlign(VAlign_Fill)
    [
        SAssignNew(MainVerticalBox, SVerticalBox)
        +SVerticalBox::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        .FillHeight(1.f)
        [
            SNew(SOverlay)
            .AddMetaData<FGraphNodeMetaData>(TagMeta)
            +SOverlay::Slot()
            .Padding(Settings->GetNonPinNodeBodyPadding())
            [
                SNew(SImage)
                .Image(GetNodeBodyBrush())
                .ColorAndOpacity(this, &SGraphNode::GetNodeBodyColor)
            ]
           +SOverlay::Slot()
           [
               InnerVerticalBox.ToSharedRef()
           ]
        ]
    ];

    CreatePinWidgets();
    CreateInputSideAddButton(LeftNodeBox);
    CreateOutputSideAddButton(RightNodeBox);
    CreateAdvancedViewArrow(InnerVerticalBox);

    if(CodePalette&&HLSLGraphNode.IsValid())
        CodePalette->SetVisibility(HLSLGraphNode->MaterialExpression->bCollapsed?EVisibility::Collapsed:EVisibility::Visible);
}

void SHLSLGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
    PinToAdd->SetOwner(SharedThis(this));

    if(PinToAdd->GetDirection()==EEdGraphPinDirection::EGPD_Input)
    {
        FMargin Padding = Settings->GetInputPinPadding();
        Padding.Left *= 0.5f;
        Padding.Right = 0.0f;

        LeftNodeBox->AddSlot()
        .AutoHeight()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Center)
        .Padding(Padding)
        [
            PinToAdd
        ];
        InputPins.Add(PinToAdd);
    }
    else // Direction == EEdGraphPinDirection::EGPD_Output
    {
        FMargin Padding = Settings->GetOutputPinPadding();
        Padding.Left = 0.0f;
        Padding.Right *= 0.5f;

        RightNodeBox->AddSlot()
        .AutoHeight()
        .HAlign(HAlign_Right)
        .VAlign(VAlign_Center)
        .Padding(Padding)
        [
            PinToAdd
        ];
        OutputPins.Add(PinToAdd);
    }
}
  
void SHLSLGraphNode::SetDefaultTitleAreaWidget(TSharedRef<SOverlay> DefaultTitleAreaWidget)
{
    DefaultTitleAreaWidget->AddSlot()
    .HAlign(HAlign_Right)
    .VAlign(VAlign_Center)
    .Padding(FMargin(5.f))
    [
        SNew(SHorizontalBox)
        +SHorizontalBox::Slot()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            SAssignNew(TitleTools, SHorizontalBox)
            .Visibility((HLSLGraphNode.IsValid()&&!HLSLGraphNode->MaterialExpression->bCollapsed)?EVisibility::Visible:EVisibility::Collapsed)
            +SHorizontalBox::Slot()
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Center)
            .Padding(FMargin(0, 0, 8.f, 0.f))
            .AutoWidth()
            [
                SNew(SButton)
                .ButtonStyle(&FHLSLCodePaletteStyle::Get().GetWidgetStyle<FButtonStyle>("HLSLPalette.Export"))
                .OnClicked(this,&SHLSLGraphNode::ExportAction)
                .ToolTipText(FText::FromString(TEXT("Export HLSL Expressions as code.")))
            ]
            +SHorizontalBox::Slot()
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Center)
            .Padding(FMargin(0,0,8.f,0.f))
            .AutoWidth()
            [
                SNew(SButton)
                .ButtonStyle(&FHLSLCodePaletteStyle::Get().GetWidgetStyle<FButtonStyle>("HLSLPalette.Import"))
                .OnClicked(this, &SHLSLGraphNode::ImportAction)
                .ToolTipText(FText::FromString(TEXT("Import HLSL Expressions.")))
            ]
            +SHorizontalBox::Slot()
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Center)
            .Padding(FMargin(0, 0, 8.f, 0.f))
            .AutoWidth()
            [
                SNew(SButton)
                .ButtonStyle(&FHLSLCodePaletteStyle::Get().GetWidgetStyle<FButtonStyle>("HLSLPalette.Save"))
                .OnClicked(this,&SHLSLGraphNode::SaveAsMaterialFunctionAction)
                .ToolTipText(FText::FromString(TEXT("Save As Material Function.")))
            ]
            +SHorizontalBox::Slot()
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Center)
            .AutoWidth()
            [
                SNew(SCheckBox)
                .OnCheckStateChanged(this, &SHLSLGraphNode::OnExpressionAutoCompileChanged)
                .IsChecked(IsAutoCompileChecked())
                .Cursor(EMouseCursor::Default)
                .ToolTipText(FText::FromString(TEXT("Whether to compile automatically when the code committed.")))
                [
                    SNew(STextBlock)
                    .ColorAndOpacity(FColor::White)
                    .Margin(0.f)
                    .Visibility(EVisibility::SelfHitTestInvisible)
                    .Text(FText::FromString(TEXT("Auto Compile")))
                ]
            ]
        ]
        +SHorizontalBox::Slot()
        .HAlign(HAlign_Right)
        .VAlign(VAlign_Center)
        .AutoWidth()
        .Padding(FMargin(6.f, 0.f, 0.f, 0.f))
        [
            SNew(SCheckBox)
            .OnCheckStateChanged(this, &SHLSLGraphNode::OnExpressionPreviewChanged)
            .IsChecked(IsExpressionPreviewChecked())
            .Cursor(EMouseCursor::Default)
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
            .Style(FAppStyle::Get(), "Graph.Node.AdvancedView")
#else
            .Style(FEditorStyle::Get(), "Graph.Node.AdvancedView")
#endif
            [
                SNew(SHorizontalBox)
                +SHorizontalBox::Slot()
                .VAlign(VAlign_Center)
                .HAlign(HAlign_Center)
                [
                    SNew(SImage)
                    .Image(GetExpressionPreviewArrow())
                ]
            ]
        ]
    ];
}

TSharedRef<SWidget> SHLSLGraphNode::CreateNodeContentArea()
{
    if(CodePalette)
    {
        TWeakObjectPtr<UMaterial> Material = HLSLGraphNode->MaterialExpression->Material;
        FHLSLTokenWatcher::GetTokenWatcher()->UnregisterCodePalette(Material, CodePalette);
    }

    UHLSLMaterialExpression* HLSLExpression = Cast<UHLSLMaterialExpression>(HLSLGraphNode->MaterialExpression);
    FString Code = HLSLExpression->GetCode();

    TWeakObjectPtr<UMaterial> Material = HLSLGraphNode->MaterialExpression->Material;
    FHLSLTokenWatcher::GetTokenWatcher()->CompareAndUpdateTokens(Material, FHLSLTokenWatcher::GetTokenWatcher()->CollectHLSLTokens(Code));

    SAssignNew(CodePalette, SHLSLCodePalette, Code, HLSLConfig::GetConfiguration()->bShowColors, FHLSLTokenWatcher::GetTokenWatcher()->GetTokens(Material))
    .OnCodeImported(this, &SHLSLGraphNode::OnCodeImported)
    .GetAdditionTokens_Lambda([Material] ()
    {
        return SHLSLCodeEditableText::MakeTokens(FHLSLTokenWatcher::GetTokenWatcher()->GetTokens(Material));
    }) 
    .OnCodeSaved(this, &SHLSLGraphNode::OnCodeSaved)
    .OnCodeChanged(this, &SHLSLGraphNode::OnCodeChanged)
    .OnCodeCommitted(this, &SHLSLGraphNode::OnCodeCommitted);
     
    FHLSLTokenWatcher::GetTokenWatcher()->RegisterCodePalette(Material, CodePalette);
	if (FHLSLTokenWatcher::GetTokenWatcher()->CompareAndUpdateTokens(Material, FHLSLTokenWatcher::GetTokenWatcher()->CollectHLSLTokens(Code)))
		FHLSLTokenWatcher::GetTokenWatcher()->UpdateCodePaletteTokens(Material);
     
    return
    SNew(SBorder)
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
    .BorderImage(FAppStyle::GetBrush("NoBorder"))
#else
    .BorderImage(FEditorStyle::GetBrush("NoBorder"))
#endif
    .HAlign(HAlign_Fill)
    .VAlign(VAlign_Fill)
    [
        SNew(SHorizontalBox)
        +SHorizontalBox::Slot()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            SAssignNew(LeftNodeBox, SVerticalBox)
        ]
        +SHorizontalBox::Slot()
        .FillWidth(1.f)
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SNew(SBox)
            .Clipping(EWidgetClipping::ClipToBoundsAlways)
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            [
                CodePalette.ToSharedRef()
            ]
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        .HAlign(HAlign_Right)
        .VAlign(VAlign_Center)
        [
            SAssignNew(RightNodeBox, SVerticalBox)
        ]
    ];
}

FReply SHLSLGraphNode::ExportAction()
{
    if(CodePalette.IsValid())
        return CodePalette->ExportAction();
    return FReply::Handled();
}

FReply SHLSLGraphNode::ImportAction()
{
    if(CodePalette.IsValid())
        return CodePalette->ImportAction();
    return FReply::Handled();
}


FReply SHLSLGraphNode::SaveAsMaterialFunctionAction()
{
    FHLSLExporter::SaveAsMaterialFunction(Cast<UHLSLMaterialExpression>(HLSLGraphNode->MaterialExpression));
    return FReply::Handled();
}

void SHLSLGraphNode::OnExpressionPreviewChanged(const ECheckBoxState NewCheckedState)
{
    if(!HLSLGraphNode.IsValid())
        return;

    UMaterialExpression* MaterialExpression = HLSLGraphNode->MaterialExpression;
    const bool bCollapsed = (NewCheckedState!=ECheckBoxState::Checked);
    if(MaterialExpression->bCollapsed!=bCollapsed)
    {
        UMaterialGraph* MaterialGraph = CastChecked<UMaterialGraph>(HLSLGraphNode->GetGraph());
        MaterialGraph->ToggleCollapsedDelegate.ExecuteIfBound(MaterialExpression);
    }

    TitleTools->SetVisibility(NewCheckedState==ECheckBoxState::Checked?EVisibility::Visible:EVisibility::Collapsed);
    CodePalette->SetVisibility(NewCheckedState==ECheckBoxState::Checked?EVisibility::Visible:EVisibility::Collapsed);
}

ECheckBoxState SHLSLGraphNode::IsExpressionPreviewChecked() const
{
    if(HLSLGraphNode.IsValid())
        return HLSLGraphNode->MaterialExpression->bCollapsed?ECheckBoxState::Unchecked:ECheckBoxState::Checked;
    return ECheckBoxState::Unchecked;
}

const FSlateBrush* SHLSLGraphNode::GetExpressionPreviewArrow() const
{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
	if (HLSLGraphNode.IsValid())
		return FAppStyle::GetBrush(HLSLGraphNode->MaterialExpression->bCollapsed ? TEXT("Icons.ChevronDown") : TEXT("Icons.ChevronUp"));
	return FAppStyle::GetBrush(TEXT("Icons.ChevronDown"));
#elif ENGINE_MAJOR_VERSION==4
	if (HLSLGraphNode.IsValid())
		return FEditorStyle::GetBrush(HLSLGraphNode->MaterialExpression->bCollapsed ? TEXT("Kismet.TitleBarEditor.ArrowDown") : TEXT("Kismet.TitleBarEditor.ArrowUp"));
	return FEditorStyle::GetBrush(TEXT("Kismet.TitleBarEditor.ArrowDown"));
#else
	if (HLSLGraphNode.IsValid())
		return FEditorStyle::GetBrush(HLSLGraphNode->MaterialExpression->bCollapsed ? TEXT("Icons.ChevronDown") : TEXT("Icons.ChevronUp"));
	return FEditorStyle::GetBrush(TEXT("Icons.ChevronDown"));
#endif
}

void SHLSLGraphNode::OnExpressionAutoCompileChanged(const ECheckBoxState NewCheckedState)
{
    if(HLSLGraphNode.IsValid())
        return Cast<UHLSLMaterialExpression>(HLSLGraphNode->MaterialExpression)->SetAutomaticCompilation(NewCheckedState==ECheckBoxState::Checked);
}

ECheckBoxState SHLSLGraphNode::IsAutoCompileChecked() const
{
    if(HLSLGraphNode.IsValid())
        return Cast<UHLSLMaterialExpression>(HLSLGraphNode->MaterialExpression)->IsAutomaticCompilation()?ECheckBoxState::Checked:ECheckBoxState::Unchecked;
    return ECheckBoxState::Unchecked;
}

void SHLSLGraphNode::ResetCode()
{
    if(!HLSLGraphNode.IsValid())
        return;

    UHLSLMaterialExpression* HLSLExpression = Cast<UHLSLMaterialExpression>(HLSLGraphNode->MaterialExpression);

    bSkipCodeChanged = true;
    CodePalette->SetCode(HLSLExpression->GetCode());
    bSkipCodeChanged = false;
}

FString SHLSLGraphNode::UpdateExpression(const FText& NewText)
{
    if(!HLSLGraphNode.IsValid())
        return TEXT("");

    UHLSLMaterialExpression* HLSLExpression = Cast<UHLSLMaterialExpression>(HLSLGraphNode->MaterialExpression);

    FString Errors;
    FHLSLExpressions NewExpressions;
    FString NewCode = NewText.ToString();

    //FHLSLContext Context = FHLSLParser::CreateContext(HLSLExpression->GetConditionOutputTypeName(),HLSLExpression->GetName(), NewText.ToString(), Errors);
    Errors = FHLSLParser::CreateExpression(HLSLExpression->GetConditionOutputTypeName(), NewCode, NewExpressions.Main, NewExpressions.Functions);

	HLSLExpression->SetCode(NewCode);
	HLSLExpression->SetExpressions(NewExpressions);

    if(Errors.IsEmpty() || 
    (HLSLExpression->ShaderFrequency==EHLSLShaderFrequency::HSF_Pixel && HLSLExpression->HLSLInputs.Num()==0 && HLSLExpression->HLSLOutputs.Num()==0))
    {
        return TEXT("");
    }
    else
    {
        HLSLUtils::HLSLCompileError(Errors);
        return Errors;
    }
}

void SHLSLGraphNode::CompileExpression(bool bForceCompile)
{
    if(!HLSLGraphNode.IsValid())
        return;

    UHLSLMaterialExpression* HLSLExpression = Cast<UHLSLMaterialExpression>(HLSLGraphNode->MaterialExpression);
    if(HLSLExpression->IsAutomaticCompilation()||bForceCompile)
        FMaterialEditorUtilities::UpdateMaterialAfterGraphChange(HLSLExpression->GraphNode->GetGraph());
}

void SHLSLGraphNode::MarkMaterialDirty()
{
    if(!HLSLGraphNode.IsValid())
        return;

    UHLSLMaterialExpression* HLSLExpression = Cast<UHLSLMaterialExpression>(HLSLGraphNode->MaterialExpression);

    HLSLExpression->MarkPackageDirty();
    if(HLSLExpression->GraphNode)
    {
        if(UMaterialGraphNode* MaterialGraphNode = Cast<UMaterialGraphNode>(HLSLExpression->GraphNode))
        {
            MaterialGraphNode->MaterialDirtyDelegate.ExecuteIfBound();
        }
    }
}

void SHLSLGraphNode::HandleCompilationErrors()
{
    if(!HLSLGraphNode.IsValid())
        return;

    UMaterial* Material = HLSLGraphNode->MaterialExpression->Material;
    FMaterialResource* Resource = Material->GetMaterialResource(GMaxRHIFeatureLevel);
    if(Resource->GetCompileErrors().Num() > 0)
    {
        MarkMaterialDirty();

        HLSLUtils::HLSLCompileError(TEXT("Clear Compile Errors by HLSLExpression's Code Changed."), Resource->GetCompileErrors());
        Resource->SetCompileErrors(TArray<FString>{});
    }
}

void SHLSLGraphNode::OnCodeSaved(const FText& NewText)
{
    UpdateExpression(NewText);
    MarkMaterialDirty();
    CompileExpression(true);
}

void SHLSLGraphNode::OnCodeChanged(const FText& NewText)
{
    if(!HLSLGraphNode.IsValid() || bSkipCodeChanged)
        return;

    bDirty = true;

    UMaterial* Material = HLSLGraphNode->MaterialExpression->Material;
    if(FHLSLTokenWatcher::GetTokenWatcher()->CompareAndUpdateTokens(Material, FHLSLTokenWatcher::GetTokenWatcher()->CollectHLSLTokens(NewText.ToString())))
        FHLSLTokenWatcher::GetTokenWatcher()->UpdateCodePaletteTokens(Material);
}

void SHLSLGraphNode::OnCodeCommitted(const FText& NewText, ETextCommit::Type Type)
{
    if(!HLSLGraphNode.IsValid())
        return;

    UHLSLMaterialExpression* HLSLExpression = Cast<UHLSLMaterialExpression>(HLSLGraphNode->MaterialExpression);
    FString Error;
    switch(Type)
    {
        case ETextCommit::Default:
        case ETextCommit::OnEnter:
        case ETextCommit::OnUserMovedFocus:
            Error = UpdateExpression(NewText);
            break;
        case ETextCommit::OnCleared:
        default:
            break;
    }

    if(bDirty)
    {
        if(HLSLExpression->FixupExpressionArguments(!Error.IsEmpty()))
        {
            bSkipCodeChanged = true;
            CodePalette->SetCode(HLSLExpression->GetCode());
            bSkipCodeChanged = false;
        }

        HandleCompilationErrors();
        MarkMaterialDirty();
        CompileExpression(false);
    }
    bDirty = false;
}

void SHLSLGraphNode::OnCodeImported(const FText& NewText)
{
    OnCodeCommitted(NewText,ETextCommit::Default);
}

