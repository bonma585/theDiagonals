// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TopDownTest : ModuleRules
{
	public TopDownTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core",
															"CoreUObject",
															"Engine",
															"InputCore",
															"NavigationSystem",
															"AIModule",
															"Niagara",
															"EnhancedInput",
															"OnlineSubsystem",
                                                            "OnlineSubsystemUtils",
                                                            "OnlineSubsystemNull",
															"OnlineSubsystemSteam" });
    }
}
