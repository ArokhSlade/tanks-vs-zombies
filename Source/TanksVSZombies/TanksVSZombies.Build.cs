// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TanksVSZombies : ModuleRules
{
	public TanksVSZombies(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Paper2D" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
		
		//https://stackoverflow.com/questions/72379796/how-to-change-include-default-file-path-for-sub-folders
		PublicIncludePaths.Add(ModuleDirectory);
	}
}
