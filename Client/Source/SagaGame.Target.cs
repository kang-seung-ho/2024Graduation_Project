// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SagaGameTarget : TargetRules
{
	public SagaGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		bUseAdaptiveUnityBuild = true;
		MinFilesUsingPrecompiledHeader = 1;

		ExtraModuleNames.AddRange(new string[]
		{
			"SagaUtility",
			"SagaFramework", "SagaAssetMgr",
			"SagaNetwork",
			"SagaGame"
		});
	}
}
