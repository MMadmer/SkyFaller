// Copyright fpwong. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintAssistTypes.h"
#include "EdGraph/EdGraphSchema.h" // EGraphType, EEdGraphPinDirection
#include "Engine/Blueprint.h" // FBPVariableDescription
#include "BlueprintAssistUtils.generated.h"

class SDockTab;
class SCommentBubble;
class IAssetEditorInstance;
struct FCommentHandler;
class IMenu;
class UEdGraphNode_Comment;
struct FGraphPinHandle;
class UEdGraphNode;
class UEdGraphPin;
class UK2Node_Knot;
class SGraphPanel;
class SGraphEditor;
class SGraphPin;
class SGraphNode;
class SWindow;
class FEdGraphFormatter;
class FBAGraphHandler;
class FBlueprintEditor;
struct FPinLink;

#define CAST_SLATE_WIDGET(Widget, WidgetClass) FBAUtils::CastWidgetByTypeName<WidgetClass>(Widget, #WidgetClass, false)
#define FIND_PARENT_WIDGET(Widget, WidgetClass) FBAUtils::CastWidgetByTypeName<WidgetClass>(FBAUtils::GetParentWidgetOfType(Widget, #WidgetClass), #WidgetClass, false)
#define FIND_CHILD_WIDGET(Widget, WidgetClass) FBAUtils::GetChildWidgetCasted<WidgetClass>(Widget, #WidgetClass)

UENUM()
enum class EBARoundingMethod : uint8
{
	Round UMETA(DisplayName = "Round"),
	Ceil UMETA(DisplayName = "Ceil"),
	Floor UMETA(DisplayName = "Floor"),
};

UENUM()
enum class EBABreakMethod : uint8
{
	Default,
	Always,
	Never
};

struct BLUEPRINTASSIST_API FBAUtils
{
	static void DeleteNode(UEdGraphNode* Node);

	static bool IsNodeDeleted(UEdGraphNode* Node);

	static bool IsNodeDeletedFromGraph(UEdGraph* Graph, UEdGraphNode* Node);

	/** For a node, return all connected nodes */
	static TArray<UEdGraphNode*> GetLinkedNodes(
		UEdGraphNode* Node,
		EEdGraphPinDirection Direction = EGPD_MAX);

	static TArray<UEdGraphNode*> GetLinkedNodesFromPins(TArray<UEdGraphPin*> Pins);

	static TArray<UEdGraphNode*> GetLinkedNodesFromPin(const UEdGraphPin* Pin);

	static bool IsNodeLinked(UEdGraphNode* Node, EEdGraphPinDirection Direction = EGPD_MAX);

	/** For a node, return all of the node's pins which have a connection */
	static TArray<UEdGraphPin*> GetLinkedPins(
		UEdGraphNode* Node,
		EEdGraphPinDirection Direction = EGPD_MAX);

	static TArray<UEdGraphPin*> GetLinkedToPins(
		UEdGraphNode* Node,
		EEdGraphPinDirection Direction = EGPD_MAX);

	/** For a node, return all linked parameters. Ignore chains of parameters which are connected to other impure (exec) nodes **/
	static TArray<UEdGraphNode*> GetNodeAndParameters(UEdGraphNode* Node);

	/** For a node, return all of the pins in a certain direction */
	static TArray<UEdGraphPin*> GetPinsByDirection(
		UEdGraphNode* Node,
		EEdGraphPinDirection Direction = EGPD_MAX);

	/** For a node, return all of the pins which are exec pins (and an optional direction) */
	static TArray<UEdGraphPin*> GetExecPins(
		UEdGraphNode* Node,
		EEdGraphPinDirection Direction = EGPD_MAX);
	
	/** For a node, return all of the pins which are exec or delegate pins (and an optional direction) */
	static TArray<UEdGraphPin*> GetExecOrDelegatePins(
		UEdGraphNode* Node,
		EEdGraphPinDirection Direction = EGPD_MAX);

	/** For a node, return all of the pins which are not exec pins (and an optional direction) */
	static TArray<UEdGraphPin*> GetParameterPins(
		UEdGraphNode* Node,
		EEdGraphPinDirection Direction = EGPD_MAX);

	/** For a node, return all of the pins in a certain direction */
	static TArray<UEdGraphPin*> GetPinsByCategory(
		UEdGraphNode* Node,
		FName Category,
		EEdGraphPinDirection Direction = EGPD_MAX);

	/** Calculate bounds for multiple FSlateRects */
	static FSlateRect GetGroupedBounds(const TArray<FSlateRect>& Rects);

	/** Get the node pos of a node as a vector */
	static FVector2D GetNodePosition(UEdGraphNode* Node);

	/** Calculate the bounds of a node */
	static FSlateRect GetNodeBounds(UEdGraphNode* Node);

	/** Calculate the bounds of a graph node */
	static FSlateRect GetNodeBounds(TSharedPtr<SGraphNode> Node);

	/** Calculate the bounds of multiple nodes */
	static FSlateRect GetNodeArrayBounds(const TArray<UEdGraphNode*>& Nodes);

	/** Calculate the bounds of a pin */
	static FSlateRect GetPinBounds(TSharedPtr<SGraphPanel> GraphPanel, UEdGraphPin* Pin);

	/** Calculate the bounds of a pin */
	static FSlateRect GetPinBounds(TSharedPtr<SGraphPin> GraphPin);

	/** Calculate the bounds of a node using the cached size in the graph handler */
	static FSlateRect GetCachedNodeBounds(TSharedPtr<FBAGraphHandler> GraphHandler, UEdGraphNode* Node, bool bWithCommentBubble = true);

	/** Calculate the bounds of an array of nodes using the cached size in the graph handler */
	static FSlateRect GetCachedNodeArrayBounds(
		TSharedPtr<FBAGraphHandler> GraphHandler,
		TArray<UEdGraphNode*> Nodes);

	/** Get the pin position, using the cached offset in the graph handler */
	static FVector2D GetPinPos(TSharedPtr<FBAGraphHandler> GraphHandler, UEdGraphPin* Pin);
	static FVector2D GetPinPos(TSharedPtr<SGraphPin> Pin);

	/** Checks all nodes on the graph and sorts them depending on whether they are on the LHS or RHS of a given node */
	static void SortNodesOnGraphByDistance(
		UEdGraphNode* RelativeNode,
		const UEdGraph* Graph,
		TSet<UEdGraphNode*>& LHSNodes,
		TSet<UEdGraphNode*>& RHSNodes,
		TSet<UEdGraphPin*>& LHSPins,
		TSet<UEdGraphPin*>& RHSPins);

	static FVector2D GetKnotNodeSize();

	/** Adds a knot node connecting two pins */
	static UK2Node_Knot* CreateKnotNode(
		UEdGraph* Graph,
		const FVector2D& Position,
		UEdGraphPin* PinA,
		UEdGraphPin* PinB);

	static void LinkKnotNodeBetween(UK2Node_Knot* Node, const FVector2D& Position, UEdGraphPin* PinA, UEdGraphPin* PinB);

	static void DisconnectKnotNode(UEdGraphNode* Node);

	static void RemoveLinkedKnotNodes(UEdGraphNode* Node, TSet<UEdGraphNode*>& RemovedNodes);

	static TArray<UEdGraphPin*> GetLinkedToPinsIgnoringKnots(UEdGraphNode* Node, EEdGraphPinDirection Direction = EGPD_MAX);

	static TArray<UEdGraphPin*> GetPinLinkedToIgnoringKnots(const UEdGraphPin* InitialPin);

	static bool IsNodePure(UEdGraphNode* Node);

	static bool IsNodeImpure(UEdGraphNode* Node);

	static bool IsKnotNode(UEdGraphNode* Node);

	static bool IsCommentNode(const UEdGraphNode* Node);

	static bool IsVarNode(UEdGraphNode* Node);

	static bool IsK2Node(UEdGraphNode* Node);

	static bool IsValidGraph(UEdGraph* Graph);

	static bool IsValidPin(UEdGraphPin* Pin);

	static bool IsPinHidden(UEdGraphPin* Pin);

	static bool IsBlueprintGraph(UEdGraph* Graph, bool bCheckFormatterSettings = true);

	static bool IsGraphNode(UEdGraphNode* Node);

	static bool IsEventNode(UEdGraphNode* Node, EEdGraphPinDirection Direction = EGPD_Output);

	static bool HasExecInOut(UEdGraphNode* Node);

	static bool IsInputNode(UEdGraphNode* Node);

	static FString GetNodeName(const UEdGraphNode* Node);

	static FString GetGraphDisplayName(const UEdGraph* Graph);

	static void PrintNodeInfo(UEdGraphNode* Node);
	static void PrintNodeArray(const TArray<UEdGraphNode*>& Nodes, const FString& InitialMessage = FString());

	static bool IsPinLinked(const UEdGraphPin* Pin);

	static bool IsPinUnlinked(const UEdGraphPin* Pin);

	static bool IsExecPin(const UEdGraphPin* Pin);

	static bool IsExecPinLink(FPinLink& PinLink);

	static bool IsExecPinLinkConst(const FPinLink& PinLink);

	static bool IsDelegatePin(const UEdGraphPin* Pin);

	static bool IsExecOrDelegatePin(const UEdGraphPin* Pin);

	static bool IsDelegatePinLinkedToCustomEvent(const UEdGraphPin* Pin);

	static bool IsExecOrDelegatePinLink(const FPinLink& PinLink);

	static bool IsParameterPin(const UEdGraphPin* Pin);

	static int GetPinIndex(UEdGraphPin* Pin);

	static int GetLinkedPinIndex(UEdGraphPin* Pin);

	static bool DoesNodeHaveExecutionTo(UEdGraphNode* NodeA, UEdGraphNode* NodeB, EEdGraphPinDirection Direction = EGPD_MAX);

	static bool DoesNodeHaveExecutionTo(UEdGraphNode* Node, const TSet<UEdGraphNode*>& NodeSet, EEdGraphPinDirection Direction = EGPD_MAX);

	static bool IsLoopingPinLink(FPinLink& PinLink, EEdGraphPinDirection Direction = EGPD_Output);

	static UEdGraphNode* GetExecutingNode(UEdGraphNode* Node);

	static TSet<UEdGraphNode*> GetNodeTreeWithFilter(
		UEdGraphNode* Node,
		TFunctionRef<bool(UEdGraphPin*)> Pred,
		EEdGraphPinDirection Direction = EGPD_MAX,
		bool bOnlyInitialDirection = false);

	static TSet<UEdGraphNode*> GetNodeTreeWithFilter(
		UEdGraphNode* Node,
		TFunctionRef<bool(const FPinLink&)> Pred,
		EEdGraphPinDirection Direction = EGPD_MAX,
		bool bOnlyInitialDirection = false);

	static TSet<UEdGraphNode*> IterateNodeTreeDepthFirst(
		UEdGraphNode* InitialNode,
		TFunctionRef<bool(const FPinLink&)> Pred,
		EEdGraphPinDirection Direction = EGPD_MAX,
		bool bOnlyInitialDirection = false);

	static TSet<UEdGraphNode*> GetNodeTree(
		UEdGraphNode* Node,
		EEdGraphPinDirection Direction = EGPD_MAX,
		bool bOnlyInitialDirection = false);

	static TSet<UEdGraphNode*> GetExecTree(
		UEdGraphNode* Node,
		EEdGraphPinDirection Direction = EGPD_MAX,
		bool bOnlyInitialDirection = false);

	static TSet<UEdGraphNode*> GetExecutionTreeWithFilter(
		UEdGraphNode* Node,
		TFunctionRef<bool(UEdGraphNode*)> Pred,
		EEdGraphPinDirection Direction = EGPD_MAX,
		bool bOnlyInitialDirection = false);

	static TSet<UEdGraphNode*> GetParameterTree(
		UEdGraphNode* Node,
		EEdGraphPinDirection Direction = EGPD_MAX,
		bool bOnlyInitialDirection = false);

	static TSet<UEdGraphNode*> GetEdGraphNodeTree(
		UEdGraphNode* Node,
		EEdGraphPinDirection Direction = EGPD_MAX);

	static UEdGraphPin* GetFirstLinkedPin(UEdGraphNode* Node, EEdGraphPinDirection Direction);

	static UEdGraphPin* GetFirstExecPin(UEdGraphNode* Node, EEdGraphPinDirection Direction);

	static UEdGraphNode* GetFirstLinkedNode(UEdGraphNode* Node, EEdGraphPinDirection Direction);

	static bool TryCreateConnection(
		FPinLink& PinLink,
		EBABreakMethod BreakMethod,
		bool bConversionAllowed = false,
		bool bTryHidden = false);

	static bool TryCreateConnection(
		FBANodePinHandle& PinA,
		FBANodePinHandle& PinB,
		EBABreakMethod BreakMethod,
		bool bConversionAllowed = false,
		bool bTryHidden = false);

	
	/**
	 * make sure to remember to check if the pin will be accessed after calling this
	 * if so you want to be using a FBANodePinHandle
	 */
	static bool TryCreateConnectionUnsafe(
		UEdGraphPin* PinA,
		UEdGraphPin* PinB,
		EBABreakMethod BreakMethod,
		bool bConversionAllowed = false,
		bool bTryHidden = false);

	static FSlateRect FSlateRectFromVectors(const FVector2D& A, const FVector2D& B);

	static int DistanceSquaredBetweenNodes(UEdGraphNode* NodeA, UEdGraphNode* NodeB);

	static bool LineRectIntersection(
		const FSlateRect& Rect,
		const FVector2D& Start,
		const FVector2D& End);

	static void SetNodePosY(TSharedPtr<FBAGraphHandler> GraphHandler, UEdGraphNode* Node, int NewY);

	static void SetNodePos(
		TSharedPtr<FBAGraphHandler> GraphHandler,
		UEdGraphNode* Node,
		int NewX,
		int NewY);

	static void SetNodePos(
		TSharedPtr<FBAGraphHandler> GraphHandler,
		UEdGraphNode* Node,
		const FVector2D& NewPos);

	static FString GraphTypeToString(EGraphType GraphType);

	static EGraphType GetGraphType(UEdGraph* Graph);

	static int StraightenPin(
		TSharedPtr<FBAGraphHandler> GraphHandler,
		UEdGraphPin* SourcePin,
		UEdGraphPin* PinToAlign);

	static float StraightenPin(
		TSharedPtr<FBAGraphHandler> GraphHandler,
		const FPinLink& PinLink);

	static bool ArePinsStraightened(TSharedPtr<FBAGraphHandler> GraphHandler, FPinLink& PinLink);

	static FSlateRect GetGraphPanelBounds(TSharedPtr<SGraphPanel> GraphPanel);

	static bool IsNodeVisible(TSharedPtr<SGraphPanel> GraphPanel, UEdGraphNode* Node);

	static FString GetPinName(UEdGraphPin* Pin, bool bIncludeOwningNode = false);

	static void PrintChildWidgets(TSharedPtr<SWidget> Widget);

	static bool IsWidgetOfType(
		TSharedPtr<SWidget> Widget,
		const FString& WidgetTypeName,
		bool bCheckContains = false);

	static bool IsWidgetOfTypeFast(TSharedPtr<SWidget> Widget, const FName& WidgetTypeName);
	static bool IsWidgetOfAnyType(TSharedPtr<SWidget> Widget, const TArray<FName>& Types);

	template <class WidgetClass>
	static TSharedPtr<WidgetClass> CastWidgetByTypeName(
		TSharedPtr<SWidget> Widget,
		const FString& WidgetTypeName,
		bool bCheckContains = false)
	{
		return IsWidgetOfType(Widget, WidgetTypeName, bCheckContains) ? StaticCastSharedPtr<WidgetClass>(Widget) : nullptr;
	}

	static TSharedPtr<SWidget> GetChildWidget(
		TSharedPtr<SWidget> Widget,
		const FString& WidgetClassName,
		bool bCheckContains = false);

	static TSharedPtr<SWidget> GetChildWidgetFast(
		TSharedPtr<SWidget> Widget,
		const FName& WidgetClassName);

	static TSharedPtr<SWidget> GetChildWidgetByTypes(
		TSharedPtr<SWidget> Widget,
		const TArray<FName>& WidgetClassNames);

	template <class WidgetClass> 
	static TSharedPtr<WidgetClass> GetChildWidgetByTypesCasted(
		TSharedPtr<SWidget> Widget,
		const TArray<FName>& WidgetClassNames)
	{
		return StaticCastSharedPtr<WidgetClass>(GetChildWidgetByTypes(Widget, WidgetClassNames));
	}

	static TSharedPtr<SWidget> GetChildWidget(
		TSharedPtr<SWidget> Widget,
		TFunctionRef<bool(TSharedPtr<SWidget>)> Pred,
		TSet<TSharedPtr<SWidget>>& Visited,
		bool bVisibleOnly = true);

	template <class WidgetClass> 
	static TSharedPtr<WidgetClass> GetChildWidgetCasted(
		TSharedPtr<SWidget> Widget,
		const FString& WidgetClassName,
		bool bCheckContains = false)
	{
		if (TSharedPtr<SWidget> ChildWidget = GetChildWidget(Widget, WidgetClassName, bCheckContains))
		{
			return StaticCastSharedPtr<WidgetClass>(ChildWidget);
		}

		return nullptr;
	}

	static void GetChildWidgets(
		TSharedPtr<SWidget> Widget,
		const FName& WidgetClassName,
		TArray<TSharedPtr<SWidget>>& OutWidgets);

	static void GetChildWidgetsByTypes(
		TSharedPtr<SWidget> Widget,
		const TArray<FName>& WidgetClassNames,
		TSet<TSharedPtr<SWidget>>& OutWidgets);

	template <class WidgetClass>
	static void GetChildWidgetsCasted(
		TSharedPtr<SWidget> Widget,
		const FName& WidgetClassName,
		TArray<TSharedPtr<WidgetClass>>& OutWidgets)
	{
		TArray<TSharedPtr<SWidget>> UncastedWidgets;
		GetChildWidgets(Widget, WidgetClassName, UncastedWidgets);

		for (TSharedPtr<SWidget> UncastedWidget : UncastedWidgets)
		{
			OutWidgets.Add(StaticCastSharedPtr<WidgetClass>(UncastedWidget));
		}
	}

	static TSharedPtr<SWindow> GetParentWindow(TSharedPtr<SWidget> Widget);
	static TSharedPtr<SWindow> GetTopMostWindow(TSharedPtr<SWindow> Window);

	static TSharedPtr<SWidget> GetParentWidgetOfType(
		TSharedPtr<SWidget> Widget,
		const FString& ParentType,
		bool bCheckContains = false);

	static TSharedPtr<SWidget> ScanParentContainersForTypes(
		TSharedPtr<SWidget> Widget,
		const TArray<FName>& Types,
		const FName& StopAtParent);

	static TSharedPtr<SGraphNode> GetGraphNode(
		TSharedPtr<SGraphPanel> GraphPanel,
		UEdGraphNode* Node);

	static TSharedPtr<SCommentBubble> GetCommentBubble(TSharedPtr<SGraphNode> GraphNode);

	static TSharedPtr<SGraphPin> GetGraphPin(TSharedPtr<SGraphPanel> GraphPanel, UEdGraphPin* Pin);

	static TSharedPtr<SGraphPanel> GetHoveredGraphPanel();

	static TSharedPtr<SGraphPin> GetHoveredGraphPin(TSharedPtr<SGraphPanel> GraphPanel);

	static TArray<TSharedPtr<SGraphPin>> GetHoveredGraphPins(TSharedPtr<SGraphPanel> GraphPanel);

	static FPinLink GetHoveredPinLink(TSharedPtr<SGraphPanel> GraphPanel);

	static UEdGraphPin* GetHoveredPin(TSharedPtr<SGraphPanel> GraphPanel);

	static TSharedPtr<SGraphNode> GetHoveredGraphNode(TSharedPtr<SGraphPanel> GraphPanel);

	static UEdGraphNode* GetHoveredNode(TSharedPtr<SGraphPanel> GraphPanel);

	static TSharedPtr<SWidget> GetLastHoveredWidget();

	static bool ContainsWidgetInFront(TSharedPtr<SWidget> Widget, const TSet<FName>& WidgetTypesInFront);

	static float SnapToGrid(const float& InFloat, EBARoundingMethod RoundingMethod = EBARoundingMethod::Round, int NumGridTiles = 1);
	static FVector2D SnapToGrid(const FVector2D& Position);
	static float AlignTo8x8Grid(const float& InFloat, EBARoundingMethod RoundingMethod = EBARoundingMethod::Ceil);

	static bool IsUserInputWidget(TSharedPtr<SWidget> Widget);

	static bool IsClickableWidget(TSharedPtr<SWidget> Widget);

	static FVector2D GraphCoordToPanelCoord(
		TSharedPtr<SGraphPanel> GraphPanel,
		const FVector2D& PanelSpaceCoordinate);

	static FVector2D PanelCoordToGraphCoord(
		TSharedPtr<SGraphPanel> GraphPanel,
		const FVector2D& PanelSpaceCoordinate);

	static FVector2D ScreenSpaceToPanelCoord(TSharedPtr<SGraphPanel> GraphPanel, const FVector2D& ScreenSpace);

	static FName GetObjectClassName(const UObject* Obj);
	static const UEdGraph* GetGraph(UEdGraphPin* Pin);
	static const UEdGraphSchema* GetSchema(UEdGraphPin* Pin);
	static const UEdGraphSchema* GetSchema(UEdGraphNode* Node);
	static void SchemaBreakPinLinks(FBANodePinHandle& Pin, bool bSendsNotification = true, bool bModify = true);
	static void SchemaBreakSinglePinLink(FBANodePinHandle& A, FBANodePinHandle& B, bool bModify = true);
	static void SchemaBreakSinglePinLink(FPinLink& PinLink, bool bModify = true);
	static void SchemaSetNodePos(TSharedPtr<SGraphPanel> GraphPanel, UEdGraphNode* Node, const FVector2D& NewPos);

	/**
	 * Use TryCreateConnection instead 
	 */
	static bool SchemaTryCreateConnection_Internal(FBANodePinHandle& A, FBANodePinHandle& B, bool bModify = true);

	static bool CanConnectPins(
		UEdGraphPin* PinA,
		UEdGraphPin* PinB,
		bool bOverrideLinks = false,
		bool bAcceptConversions = false,
		bool bAcceptHiddenPins = false);

	static void InteractWithWidget(TSharedPtr<SWidget> Widget);

	static void TryClickOnWidget(TSharedPtr<SWidget> Widget);

	static TSharedPtr<SWidget> GetInteractableChildWidget(TSharedPtr<SWidget> Widget);

	static bool DoesPinTypeCategoryObjectMatchName(const UEdGraphPin* Pin, const FName& StructName);

	static void GetInteractableChildWidgets(
		TSharedPtr<SWidget> Widget,
		TArray<TSharedPtr<SWidget>>& OutWidgets,
		bool bCheckEditable = true,
		bool bCheckClickable = true);

	static UEdGraphNode* GetTopMost(UEdGraphNode* Node, EEdGraphPinDirection Direction);

	static UEdGraphNode* GetTopMostWithFilter(
		UEdGraphNode* Node,
		EEdGraphPinDirection Direction,
		TFunctionRef<bool (UEdGraphNode*)> Pred);

	static void SafeDelete(TSharedPtr<FBAGraphHandler> GraphHandler, UEdGraphNode* Node);

	static bool IsPinLinkedToNode(UEdGraphPin* Pin, UEdGraphNode* Node, const TSet<UEdGraphNode*>& IgnoredNodes);

	static TArray<FPinLink> GetPinLinks(UEdGraphNode* Node, EEdGraphPinDirection Direction = EGPD_MAX);

	static TArray<FPinLink> GetPinLinksIgnoringKnots(UEdGraphNode* Node, EEdGraphPinDirection Direction = EGPD_MAX);

	static UEdGraphPin* GetPinFromGraph(const FGraphPinHandle& Handle, UEdGraph* Graph);

	static UEdGraphNode* GetNodeFromGraph(const UEdGraph* Graph, const FGuid& NodeGuid);

	static bool IsExtraRootNode(UEdGraphNode* Node);

	static void SwapNodes(UEdGraphNode* NodeA, UEdGraphNode* NodeB);

	static TArray<UEdGraphNode*> GetNodesUnderComment(UEdGraphNode_Comment* CommentNode);

	template<class T>
	static TArray<T*> GetNodesOfClass(TArray<UEdGraphNode*> Nodes)
	{
		TArray<T*> OutNodes;
		for (UEdGraphNode* Node : Nodes)
		{
			if (auto Casted = Cast<T>(Node))
			{
				OutNodes.Add(Casted);
			}
		}

		return OutNodes;
	}

	static TArray<UEdGraphNode_Comment*> GetCommentNodesFromGraph(UEdGraph* Graph);

	static TArray<UEdGraphNode_Comment*> GetContainingCommentNodes(const TArray<UEdGraphNode_Comment*>& Comments, UEdGraphNode* Node);

	static void MoveComment(UEdGraphNode_Comment* Comment, FVector2D Delta);

	static FSlateRect GetCommentBounds(FCommentHandler* CommentHandler, UEdGraphNode_Comment* CommentNode, UEdGraphNode* NodeAsking = nullptr);

	static FSlateRect GetCachedNodeArrayBoundsWithComments(TSharedPtr<FBAGraphHandler> GraphHandler, FCommentHandler* CommentHandler, const TArray<UEdGraphNode*>& Nodes, UEdGraphNode* NodeAsking = nullptr);

	static UEdGraphNode* GetFirstLinkedNodePreferringInput(UEdGraphNode* Node);

	template <class T>
	static bool DoesArrayContainsAllItems(const TArray<T>& Array, const TArray<T>& Items)
	{
		for (T Item : Items)
		{
			if (!Array.Contains(Item))
			{
				return false;
			}
		}

		return true;
	}

	static void RemoveNodeFromComment(UEdGraphNode_Comment* Comment, UEdGraphNode* NodeToRemove);

	static TSharedPtr<FBAGraphHandler> GetCurrentGraphHandler();

	static float GetCenterYOfPins(TSharedPtr<FBAGraphHandler> GraphHandler, TArray<UEdGraphPin*>& Pins);

	static bool IsCompilingCode();

	static TSharedPtr<IMenu> OpenPopupMenu(
		TSharedRef<SWidget> WindowContents,
		const FVector2D& PopupDesiredSize,
		const FVector2D& WindowOffset = FVector2D::ZeroVector,
		const FVector2D& MenuAnchor = FVector2D(0.5f, 0.5f));

	static TSharedPtr<class SGraphActionMenu> GetGraphActionMenu();

	static bool HasPropertyFlags(const FBPVariableDescription& Variable, uint64 FlagsToCheck);

	static bool HasMetaDataChanged(const FBPVariableDescription& OldVariable, const FBPVariableDescription& NewVariable, const FName Key);

	static IAssetEditorInstance* GetEditorFromActiveTab();

	static TSharedPtr<SDockTab> FindParentTabForWidget(TSharedPtr<SWidget> Widget);

	static UBlueprint* GetBlueprintFromGraph(const UEdGraph* Graph);

	static FBlueprintEditor* GetBlueprintEditorForGraph(const UEdGraph* Graph);

	static UMetaData* GetNodeMetaData(UEdGraphNode* Node);
	static FString GetVariableName(const FString& Name, const FName& PinCategory, EPinContainerType ContainerType);

	static UEdGraphPin* GetKnotPinByDirection(UK2Node_Knot* KnotNode, EEdGraphPinDirection Direction);

	static bool IsGamePlayingAndHasFocus();

	static FGuid GetNodeGuid(UEdGraphNode* Node);

	static const TArray<FName>& GetSearchBoxNames();

	static FGuid GetGraphGuid(UEdGraph* Graph);

	static bool IsNodeBeingRenamed(TSharedPtr<SGraphNode> GraphNode);
	static EEdGraphPinDirection GetOppositeDirection(EEdGraphPinDirection Direction);

	static bool IsObjectPinType(const FName& PinCategory);
	static FString GetDefaultPinValue(UEdGraphPin* Pin);
	static FString AttachPropertyNamesToValue(const FString& DefaultValue, const UScriptStruct* StructType);
	static bool TrySetDefaultPinValues(UEdGraphPin* Pin, const FString& NewDefaultValue, UObject* NewDefaultObject, const FText& NewDefaultTextValue, FString* OutError = nullptr);
	static bool TrySetDefaultPinValuesFromString(UEdGraphPin* Pin, const FString& NewDefault, FString* OutError = nullptr);

	static UEdGraphPin* FindSelfPin(UEdGraphNode* Node);

	static UEdGraphPin* GetFirstValuePinOnNode(TSharedPtr<FBAGraphHandler> GraphHandler, UEdGraphNode* Node);

	static bool GetPinOffset(TSharedPtr<SGraphPanel> GraphPanel, UEdGraphPin* Pin, FVector2D& OutPinOffset);
	static TFunction<bool(UEdGraphPin&, UEdGraphPin&)> GetHighestPinPredicate(TSharedPtr<SGraphPanel> GraphPanel);

	static bool IsPinVisible(UEdGraphPin* Pin);
	static FText GetNodeTitle(UEdGraphNode* Node);

	static UPackage* GetPackage(UObject* Obj);
};
