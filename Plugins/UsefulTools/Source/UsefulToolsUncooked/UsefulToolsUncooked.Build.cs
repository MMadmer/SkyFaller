using UnrealBuildTool;

public class UsefulToolsUncooked : ModuleRules
{
	public UsefulToolsUncooked(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"BlueprintGraph",
				"EnhancedInput"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"KismetCompiler",
				"UnrealEd"
			}
		);
	}
}