// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StellarFever : ModuleRules
{
    public StellarFever(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AIModule", 
                                                            "UMG", "HeadMountedDisplay", "MoviePlayer",
                                                            "GameplayTasks", "NavigationSystem", "Niagara", "Slate" });

        // Uncomment if you are using Slate UI
        PrivateDependencyModuleNames.AddRange(new string[] { "SlateCore" });

        // Uncomment if you are using online features
        PublicDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemNull" });

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
        DynamicallyLoadedModuleNames.AddRange(
                new string[]
                {
					// ... add any modules that your module loads dynamically here ...
					"OnlineSubsystemSteam"
                }
                );
    }
}