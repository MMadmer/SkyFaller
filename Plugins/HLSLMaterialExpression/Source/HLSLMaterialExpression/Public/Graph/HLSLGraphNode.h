// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Layout/Visibility.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "Types/SlateStructs.h"
#include "SGraphNodeResizable.h"
/**
* 
*/

enum class EHLSLShaderFrequency : uint8;
class SHLSLGraphNode : public SGraphNodeResizable
{
public:
    SLATE_BEGIN_ARGS(SHLSLGraphNode) {}
    SLATE_END_ARGS()

    ~SHLSLGraphNode();
    void Construct(const FArguments& InArgs, class UMaterialGraphNode* Node);
protected:
    //~ Begin SWidget Interface
    FReply OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
    FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
    FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    //~ End SWidget Interface

    //~ Begin SNodePanel::SNode Interface
    void EndUserInteraction() const override;
    //~ End SNodePanel::SNode Interface

    //~ Begin SPanel Interface
    FVector2D ComputeDesiredSize(float Scale) const override;
    //~ End SPanel Interface

    //~ Begin SGraphNode Interface
    bool IsNameReadOnly() const override;
    //~ End SGraphNode Interface

    bool CanBeSelected(const FVector2D& MousePositionInNode) const override{return true;};
protected:
    //~ Begin SGraphNodeResizable Interface
    FSlateRect GetHitTestingBorder() const override;
    FVector2D GetNodeMaximumSize() const override;
    //~ Begin SGraphNodeResizable Interface
protected:
    //~ Begin SGraphNode Interface
    void UpdateGraphNode()override;
    void CreatePinWidgets() override;
#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
    void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty = true) override;
#else
    void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter) override;
#endif
    //~ End SGraphNode Interface
protected:
    bool ShouldAllowCulling()const override{return false;};
    void AddPin(const TSharedRef<class SGraphPin>& PinToAdd) override;
    void SetDefaultTitleAreaWidget(TSharedRef<class SOverlay> DefaultTitleAreaWidget) override;
    TSharedRef<SWidget> CreateNodeContentArea() override;
protected:
    FReply ExportAction();
    FReply ImportAction();
    FReply SaveAsMaterialFunctionAction();

    void OnExpressionPreviewChanged(const ECheckBoxState NewCheckedState);
    ECheckBoxState IsExpressionPreviewChecked() const;
    const FSlateBrush* GetExpressionPreviewArrow() const;

    void OnExpressionAutoCompileChanged(const ECheckBoxState NewCheckedState);
    ECheckBoxState IsAutoCompileChecked() const;
private:
    void ResetCode();
    FString UpdateExpression(const FText& NewText);
    void CompileExpression(bool bForceCompile = false);
    void MarkMaterialDirty();
    void HandleCompilationErrors();
    void OnCodeSaved(const FText& NewText);
    void OnCodeChanged(const FText& NewText);
    void OnCodeCommitted(const FText& NewText, ETextCommit::Type Type);
    void OnCodeImported(const FText& NewText);
    bool bSkipCodeChanged = false;
    bool bDirty = false;
private:
    TSharedPtr<class SWidget> TitleTools = nullptr;
    TSharedPtr<class SHLSLCodePalette> CodePalette = nullptr;
    TWeakObjectPtr<class UMaterialGraphNode> HLSLGraphNode = nullptr;
};