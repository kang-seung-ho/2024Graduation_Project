// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SagaNetwork : ModuleRules
{
	public SagaNetwork(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { "SagaNetwork" });
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Networking", "Sockets" });

		PublicDependencyModuleNames.AddRange(new string[] { "SagaUtility" });

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"SagaUtility",
			"SagaFramework",
			"SagaGame"
		});
	}
}
