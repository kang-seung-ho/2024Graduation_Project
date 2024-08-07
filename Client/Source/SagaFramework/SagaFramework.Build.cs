// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SagaFramework : ModuleRules
{
	public SagaFramework(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { "SagaFramework" });

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate", "SlateCore", "UMG"
		});

		//PrivateDependencyModuleNames.AddRange(new string[] { "StateTree" });

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"SagaUtility"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"SagaAssetMgr",
			"SagaGame",
			"SagaNetwork"
		});
	}
}
