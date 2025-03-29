// Copyright 2023 ZSttt, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HLSLMaterialExpression : ModuleRules
{
	public HLSLMaterialExpression(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
        new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "Projects",
            "Renderer",
            "UnrealEd",
            "RHI",
            "RenderCore",
            "Slate",
            "SlateCore",
            "InputCore",
            "EditorStyle",
            "MessageLog",
            "DesktopPlatform",
            "MaterialEditor",
            "DeveloperSettings",
            "ShaderCompilerCommon",
            "GraphEditor",
            "JsonUtilities",
            "Json",
            "AppFramework",
            "AssetRegistry",
        });
    }
}
