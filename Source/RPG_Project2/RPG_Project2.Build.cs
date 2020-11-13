// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RPG_Project2 : ModuleRules
{
	public RPG_Project2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", 
			"Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule" });
	}
}
