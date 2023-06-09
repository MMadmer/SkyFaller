// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SkyFaller : ModuleRules
{
	public SkyFaller(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"Niagara"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicIncludePaths.AddRange(new string[]
		{
			"SkyFaller/Public",
			"SkyFaller/Public/Player",
			"SkyFaller/Public/Player/Weapon",
			"SkyFaller/Public/Components",
			"SkyFaller/Public/Objects",
			"SkyFaller/Public/SaveSys",
			"SkyFaller/Public/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
