// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SagaGame : ModuleRules
{
	public SagaGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { "SagaGame" });

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG" });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "Networking", "Sockets", "OnlineSubsystem" });

		PublicDependencyModuleNames.AddRange(new string[] { "AIModule" });

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"SagaUtility",
			"SagaFramework",
			"SagaAssetMgr"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"SagaNetwork"
		});
	}
}
