// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SagaGameEditorTarget : TargetRules
{
	public SagaGameEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
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
