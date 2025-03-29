// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FHLSLMaterialExpressionModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
protected:
	FString FindPluginPath();
public:
	static FString HME_PluginPath;
};
