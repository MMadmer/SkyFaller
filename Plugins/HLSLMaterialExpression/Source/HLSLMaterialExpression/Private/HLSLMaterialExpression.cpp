// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#include "HLSLMaterialExpression.h"
#include "Graph/HLSLGraphFactories.h"
#include "HLSL/HLSLStyle.h"
#include "HLSL/HLSLExporter.h"
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "FHLSLMaterialExpressionModule"

FString FHLSLMaterialExpressionModule::HME_PluginPath = TEXT("");

FString FHLSLMaterialExpressionModule::FindPluginPath()
{
    if (FPaths::DirectoryExists(FPaths::EnginePluginsDir() / TEXT("HLSLMaterialExpression")))
        return FPaths::EnginePluginsDir() / TEXT("HLSLMaterialExpression");
	else if (FPaths::DirectoryExists(FPaths::EnginePluginsDir() / TEXT("Marketplace/HLSLMaterialExpression")))
		return FPaths::EnginePluginsDir() / TEXT("Marketplace/HLSLMaterialExpression");
	else if (FPaths::DirectoryExists(FPaths::ProjectPluginsDir() / TEXT("HLSLMaterialExpression")))
		return FPaths::ProjectPluginsDir() / TEXT("HLSLMaterialExpression");
    return FPaths::ProjectPluginsDir() / TEXT("HLSLMaterialExpression");
}

void FHLSLMaterialExpressionModule::StartupModule()
{
    HME_PluginPath = FindPluginPath();
    FHLSLCodePaletteStyle::Initialize();
    FHLSLExpressionMenuFactory::RegisterHLSLExpressionMenus();
    FHLSLExpressionGraphNodeFactory::RegisterFactories();
    FHLSLExporter::Register();
}

void FHLSLMaterialExpressionModule::ShutdownModule()
{
    FHLSLCodePaletteStyle::Shutdown();
    FHLSLExpressionMenuFactory::UnregisterHLSLExpressionMenus();
    FHLSLExpressionGraphNodeFactory::UnRegisterFactories();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHLSLMaterialExpressionModule, HLSLMaterialExpression)