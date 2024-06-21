// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class victroyShip : ModuleRules
{
	public victroyShip(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "HTTP", "AudioCapture", "UMG", "Json", "AIModule", "NavigationSystem", "GameplayTasks", "MediaAssets", "Niagara", "LevelSequence", "MovieScene" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
    }
}
