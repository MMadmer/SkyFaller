// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 2
#include "EdGraph/EdGraphNode.h"
#else
#include "Engine/Classes/EdGraph/EdGraphNode.h"
#endif
#include "SGraphNode.h"

struct FHLSLExpressionMenuFactory
{
    static void RegisterHLSLExpressionMenus();
    static void UnregisterHLSLExpressionMenus();
    static class FDelegateHandle RegisterHandle;
};

struct FHLSLExpressionGraphNodeFactory : public FGraphPanelNodeFactory
{
    virtual TSharedPtr<class SGraphNode> CreateNode(class UEdGraphNode* InNode) const override;
    static void RegisterFactories();
    static void UnRegisterFactories();
};