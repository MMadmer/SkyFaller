// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#include "Graph/HLSLGraphFactories.h"
#include "Graph/HLSLGraphNode.h"
#include "HLSL/Expression.h"
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 2
#include "MaterialGraph/MaterialGraphNode.h"
#else
#include "UnrealEd/Classes/MaterialGraph/MaterialGraphNode.h"
#endif
#include "Templates/SharedPointer.h"
#include "Internationalization/Internationalization.h"
#include "IMaterialEditor.h"
#include "MaterialEditorModule.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/Commands/UICommandList.h"
#include "Delegates/IDelegateInstance.h"

#include "Editor.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Editor/UnrealEd/Classes/MaterialEditor/PreviewMaterial.h"
#include "Editor/MaterialEditor/Private/MaterialEditor.h"
#include "GraphEditor.h"
#include "NodeFactory.h"
#include "EdGraph/EdGraphNode.h"


#include "HLSL/HLSLStyle.h"
#include "Widgets/SHLSLStyleSetup.h"

class FHLSLGraphNodeFactory : public FGraphNodeFactory
{
public:
    TSharedPtr<SGraphNode> CreateNodeWidget(UEdGraphNode* InNode) override
    {
        if (UMaterialGraphNode* MaterialNode = Cast<UMaterialGraphNode>(InNode))
        {
            if (UHLSLMaterialExpression* HLSL = Cast<UHLSLMaterialExpression>(MaterialNode->MaterialExpression))
            {
                if (HLSL->GetCodePaletteSize().X == 0.f && HLSL->GetCodePaletteSize().Y == 0.f)
                {
                    HLSL->SetCodePaletteSize(FIntPoint(HLSL_EXPRESSION_NODE_DEFAULT_SIZE.X, HLSL_EXPRESSION_NODE_DEFAULT_SIZE.Y));
                    HLSL->SetCodePalettePosition(FIntPoint(InNode->NodePosX - (HLSL->GetCodePaletteSize().X * 0.25f), InNode->NodePosY - (HLSL->GetCodePaletteSize().Y * 0.25f)));
                }
                InNode->NodeWidth = HLSL->GetCodePaletteSize().X;
                InNode->NodeHeight = HLSL->GetCodePaletteSize().Y;
                InNode->NodePosX = HLSL->GetCodePalettePosition().X;
                InNode->NodePosY = HLSL->GetCodePalettePosition().Y;
                //HLSL->GraphNode->ReconstructNode();

                TSharedPtr<SHLSLGraphNode> HLSLNode;
                SAssignNew(HLSLNode, SHLSLGraphNode, MaterialNode);

                return HLSLNode;
            }
        }
        return FGraphNodeFactory::CreateNodeWidget(InNode);
    };
};

static TSharedPtr<FHLSLGraphNodeFactory> GlobalHLSLGraphNodeFactory = MakeShared<FHLSLGraphNodeFactory>();
static TArray<TWeakObjectPtr<UMaterial>> EditorMaterials;
FDelegateHandle FHLSLExpressionMenuFactory::RegisterHandle = FDelegateHandle();
void FHLSLExpressionMenuFactory::RegisterHLSLExpressionMenus()
{
    GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetOpenedInEditor().AddLambda([](UObject* Object, IAssetEditorInstance* AssetEditorInstance)
    {
        FString Name = AssetEditorInstance->GetEditorName().ToString();
        if (Name == TEXT("MaterialEditor"))
        {
            FMaterialEditor* MatEditor = (FMaterialEditor*)AssetEditorInstance;
            if(UMaterial* Material = Cast<UMaterial>(MatEditor->Material))
                EditorMaterials.AddUnique(Material);
        }
    });


    GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetEditorOpened().AddLambda([](UObject* Object)
    {
        for (auto It = EditorMaterials.CreateIterator(); It; ++It)
        {
            if(It->IsValid())
			{
				TSharedPtr<SGraphEditor> Editor = SGraphEditor::FindGraphEditorForGraph((*It)->MaterialGraph);
				if (Editor.IsValid())
				{
					Editor->SetNodeFactory(GlobalHLSLGraphNodeFactory.ToSharedRef());
				}
			}
            It.RemoveCurrent();
        }
    });


    /*GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnEditorOpeningPreWidgets().AddLambda([](const TArray<UObject*>& Objects, IAssetEditorInstance* AssetEditorInstance)
    {
        for (UObject* Object : Objects)
        {
            check(Object);
        }
    });*/

    GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetEditorRequestedOpen().AddLambda([](UObject* Object)
    {
        check(Object);
    });

    RegisterHandle = IMaterialEditorModule::Get().OnMaterialEditorOpened().AddLambda([](TWeakPtr<IMaterialEditor> MaterialEditor)
    {
        if(MaterialEditor.IsValid())
        {
            MaterialEditor.Pin()->OnRegisterTabSpawners().AddLambda([MaterialEditor](const TSharedRef<class FTabManager>&TabManager)
            {
                TSharedPtr<SGraphEditor> Editor = SGraphEditor::FindGraphEditorForGraph(Cast<UMaterial>(MaterialEditor.Pin()->GetMaterialInterface())->MaterialGraph);
                //check(!Editor.IsValid());

                auto ParentCategoryRef = TabManager->GetLocalWorkspaceMenuRoot();
                const FName TabId( TEXT( "MaterialEditor_HLSLCodeStyle" ) );
                TabManager->RegisterDefaultTabWindowSize(TabId,FVector2D(400.f,450.f));

                TabManager->RegisterTabSpawner(TabId, 
                FOnSpawnTab::CreateLambda([](const FSpawnTabArgs& Args)
                {
                    return
                    SNew(SDockTab)
                    .Label(FText::FromString(TEXT("HLSL Style Setup")))
                    .ShouldAutosize(false)
		            [
                        SNew(SHLSLStyleSetup)
		            ];
                }))
                .SetDisplayName(FText::FromString(TEXT("HLSL Expression")))
                .SetGroup(ParentCategoryRef)
                .SetIcon(FSlateIcon(FHLSLCodePaletteStyle::GetStyleSetName(), "HLSLPalette.Settings"));
            });              
        }
    });
}

void FHLSLExpressionMenuFactory::UnregisterHLSLExpressionMenus()
{
    if(RegisterHandle.IsValid())
        IMaterialEditorModule::Get().OnMaterialEditorOpened().Remove(RegisterHandle);
}

static TSharedPtr<FHLSLExpressionGraphNodeFactory> HLSLGraphNodeFactory = nullptr;

TSharedPtr<class SGraphNode> FHLSLExpressionGraphNodeFactory::CreateNode(class UEdGraphNode* InNode) const
{
    if(UMaterialGraphNode* Material = Cast<UMaterialGraphNode>(InNode))
    {
        if(UHLSLMaterialExpression* HLSL = Cast<UHLSLMaterialExpression>(Material->MaterialExpression))
        {
            if(HLSL->GetCodePaletteSize().X==0.f && HLSL->GetCodePaletteSize().Y==0.f)
            {
                HLSL->SetCodePaletteSize(FIntPoint(HLSL_EXPRESSION_NODE_DEFAULT_SIZE.X, HLSL_EXPRESSION_NODE_DEFAULT_SIZE.Y));
                HLSL->SetCodePalettePosition(FIntPoint(InNode->NodePosX-(HLSL->GetCodePaletteSize().X * 0.25f), InNode->NodePosY-(HLSL->GetCodePaletteSize().Y * 0.25f)));
            }
            InNode->NodeWidth = HLSL->GetCodePaletteSize().X;
            InNode->NodeHeight = HLSL->GetCodePaletteSize().Y;
            InNode->NodePosX = HLSL->GetCodePalettePosition().X;
            InNode->NodePosY = HLSL->GetCodePalettePosition().Y;
            //HLSL->GraphNode->ReconstructNode();
            return SNew(SHLSLGraphNode, Material);
        }
    }
    return nullptr;
}

void FHLSLExpressionGraphNodeFactory::RegisterFactories()
{
    if(!HLSLGraphNodeFactory.IsValid())
    {
        HLSLGraphNodeFactory = MakeShared<FHLSLExpressionGraphNodeFactory>();
        FEdGraphUtilities::RegisterVisualNodeFactory(HLSLGraphNodeFactory);
    }
}

void FHLSLExpressionGraphNodeFactory::UnRegisterFactories()
{
    if(HLSLGraphNodeFactory.IsValid())
    {
        FEdGraphUtilities::UnregisterVisualNodeFactory(HLSLGraphNodeFactory);
        HLSLGraphNodeFactory.Reset();
    }
}

