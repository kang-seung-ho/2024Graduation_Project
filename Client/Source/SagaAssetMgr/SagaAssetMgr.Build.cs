// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SagaAssetMgr : ModuleRules
{
	public SagaAssetMgr(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { "SagaAssetMgr" });
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "UMG" });

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"SagaUtility"
		});
	}
}
