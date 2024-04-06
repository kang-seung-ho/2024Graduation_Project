// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SagaUtility : ModuleRules
{
	public SagaUtility(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { "SagaUtility" });
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
	}
}
