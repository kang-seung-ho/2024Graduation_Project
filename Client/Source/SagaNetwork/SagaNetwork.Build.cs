// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SagaNetwork : ModuleRules
{
	public SagaNetwork(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { "SagaNetwork" });

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "DeveloperSettings" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Networking", "Sockets" });

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"SagaUtility",
			"SagaGame",
			"SagaFramework"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"SagaUtility"
		});
	}
}
