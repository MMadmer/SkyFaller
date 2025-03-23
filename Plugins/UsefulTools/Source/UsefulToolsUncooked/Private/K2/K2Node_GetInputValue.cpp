// Fill out your copyright notice in the Description page of Project Settings.

#include "K2/K2Node_GetInputValue.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "KismetCompiler.h"
#include "K2Node_CallFunction.h"
#include "EnhancedInputLibrary.h"

#define LOCTEXT_NAMESPACE "K2Node"

void UK2Node_GetInputValue::PostReconstructNode()
{
	Super::PostReconstructNode();

	RefreshOutputPinType();
}

void UK2Node_GetInputValue::PinDefaultValueChanged(UEdGraphPin* ChangedPin)
{
	if (ChangedPin == GetActionPin())
	{
		RefreshOutputPinType();
	}
}

void UK2Node_GetInputValue::AllocateDefaultPins()
{
	/** Creates input pins for ActionValue and Action, and an output pin for Value. */
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FInputActionValue::StaticStruct(), TEXT("ActionValue"));
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UInputAction::StaticClass(), TEXT("Action"));
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, TEXT("Value"));

	Super::AllocateDefaultPins();
}

void UK2Node_GetInputValue::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	/** Registers this node in the blueprint action database. */
	if (!ActionRegistrar.IsOpenForRegistration(GetClass())) { return; }

	UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
	check(NodeSpawner != nullptr);

	NodeSpawner->NodeClass = GetClass();

	ActionRegistrar.AddBlueprintAction(GetClass(), NodeSpawner);
}

void UK2Node_GetInputValue::RefreshOutputPinType()
{
	/** Updates the output pin type based on the selected Action's ValueType. */
	UEdGraphPin* OutputPin = GetOutputPin();
	if (!OutputPin) return;

	OutputPin->Modify();

	// Resets pin type before updating
	OutputPin->PinType = FEdGraphPinType();

	const UEdGraphPin* ActionPin = GetActionPin();
	if (!ActionPin || !ActionPin->DefaultObject)
	{
		OutputPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
		return;
	}

	const UInputAction* InputAction = Cast<UInputAction>(ActionPin->DefaultObject);
	if (!InputAction) return;

	switch (InputAction->ValueType)
	{
	case EInputActionValueType::Boolean:
		OutputPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
		break;
	case EInputActionValueType::Axis1D:
		OutputPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Float;
		OutputPin->PinType.PinSubCategory = TEXT("double");
		break;
	case EInputActionValueType::Axis2D:
		OutputPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		OutputPin->PinType.PinSubCategoryObject = TBaseStructure<FVector2D>::Get();
		break;
	case EInputActionValueType::Axis3D:
		OutputPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		OutputPin->PinType.PinSubCategoryObject = TBaseStructure<FVector>::Get();
		break;
	default:
		OutputPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
		break;
	}

	// Notifies the system about pin type changes
	GetSchema()->ForceVisualizationCacheClear();
	GetGraph()->NotifyGraphChanged();
}

void UK2Node_GetInputValue::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	/** Expands the node into a function call from UEnhancedInputLibrary. */
	Super::ExpandNode(CompilerContext, SourceGraph);

	UEdGraphPin* ActionValuePin = GetActionValuePin();
	const UEdGraphPin* ActionPin = GetActionPin();
	UEdGraphPin* OutputPin = GetOutputPin();

	if (!ActionValuePin || !OutputPin)
	{
		CompilerContext.MessageLog.Error(
			*LOCTEXT("MissingPins", "GetInputValue: Missing pins").ToString(), this);
		return;
	}

	if (!ActionPin || !ActionPin->DefaultObject)
	{
		CompilerContext.MessageLog.Error(
			*LOCTEXT("InvalidInputAction", "GetInputValue: Action pin is invalid or not set").ToString(), this);
		return;
	}

	const UInputAction* InputAction = Cast<UInputAction>(ActionPin->DefaultObject);
	if (!InputAction)
	{
		CompilerContext.MessageLog.Error(
			*LOCTEXT("InvalidInputAction", "GetInputValue: Action pin does not contain a valid InputAction").ToString(),
			this);
		return;
	}

	// Determines which function to call based on ValueType
	FName FunctionName;
	FName ActionValueName = TEXT("InValue");
	switch (InputAction->ValueType)
	{
	case EInputActionValueType::Boolean:
		FunctionName = GET_FUNCTION_NAME_CHECKED(UEnhancedInputLibrary, Conv_InputActionValueToBool);
		break;
	case EInputActionValueType::Axis1D:
		FunctionName = GET_FUNCTION_NAME_CHECKED(UEnhancedInputLibrary, Conv_InputActionValueToAxis1D);
		break;
	case EInputActionValueType::Axis2D:
		FunctionName = GET_FUNCTION_NAME_CHECKED(UEnhancedInputLibrary, Conv_InputActionValueToAxis2D);
		break;
	case EInputActionValueType::Axis3D:
		FunctionName = GET_FUNCTION_NAME_CHECKED(UEnhancedInputLibrary, Conv_InputActionValueToAxis3D);
		ActionValueName = TEXT("ActionValue");
		break;
	default:
		CompilerContext.MessageLog.Error(
			*LOCTEXT("UnsupportedType", "GetInputValue: Unsupported Action Value Type").ToString(), this);
		return;
	}

	if (OutputPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
	{
		CompilerContext.MessageLog.Error(
			*LOCTEXT("WildcardError", "GetInputValue: Output pin type is still Wildcard!").ToString(), this);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("OutputPin Type: %s"), *OutputPin->PinType.PinCategory.ToString());

	// Creates a CallFunction node for the selected function
	UK2Node_CallFunction* GetValueNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	if (!GetValueNode)
	{
		CompilerContext.MessageLog.Error(
			*LOCTEXT("NodeSpawnError", "GetInputValue: Failed to create intermediate function node").ToString(), this);
		return;
	}
	GetValueNode->FunctionReference.SetExternalMember(FunctionName, UEnhancedInputLibrary::StaticClass());
	GetValueNode->AllocateDefaultPins();

	// Ensures the function has the correct input pin
	UEdGraphPin* InValuePin = GetValueNode->FindPin(ActionValueName);
	if (!InValuePin)
	{
		CompilerContext.MessageLog.Error(
			*LOCTEXT("MissingInputPin", "GetInputValue: Could not find expected input pin on GetValueNode").ToString(),
			this);
		return;
	}

	// Moves links from ActionValuePin -> InValuePin
	CompilerContext.MovePinLinksToIntermediate(*ActionValuePin, *InValuePin);

	// Moves links from OutputPin -> GetValueNode output
	CompilerContext.MovePinLinksToIntermediate(*OutputPin, *GetValueNode->GetReturnValuePin());

	// Breaks all links on this node since it's no longer needed
	BreakAllNodeLinks();
}

#undef LOCTEXT_NAMESPACE
