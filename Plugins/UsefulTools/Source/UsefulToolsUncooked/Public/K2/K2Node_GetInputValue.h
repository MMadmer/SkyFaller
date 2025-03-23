// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_GetInputValue.generated.h"

UCLASS()
class UK2Node_GetInputValue : public UK2Node
{
	GENERATED_BODY()

public:
	//~UEdGraphNode interface
	virtual void PostReconstructNode() override;
	virtual void PinDefaultValueChanged(UEdGraphPin* ChangedPin) override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	//~End of UEdGraphNode interface

	//~UK2Node interface
	virtual void AllocateDefaultPins() override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual bool IsNodePure() const override { return true; }

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return NSLOCTEXT("K2Node", "GetInputValue", "Get Input Value");
	}

	virtual FText GetMenuCategory() const override { return NSLOCTEXT("K2Node", "InputCategory", "Enhanced Input"); }
	//~End of UK2Node interface

private:
	void RefreshOutputPinType();

	UEdGraphPin* GetActionValuePin() const { return FindPinChecked(TEXT("ActionValue")); }
	UEdGraphPin* GetActionPin() const { return FindPinChecked(TEXT("Action")); }
	UEdGraphPin* GetOutputPin() const { return FindPinChecked(TEXT("Value")); }
};
