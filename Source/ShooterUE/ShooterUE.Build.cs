// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShooterUE : ModuleRules
{
	public ShooterUE(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ShooterUE",
			"ShooterUE/Movement",
			"ShooterUE/Variant_Horror",
			"ShooterUE/Variant_Horror/UI",
			"ShooterUE/Variant_Shooter",
			"ShooterUE/Variant_Shooter/AI",
			"ShooterUE/Variant_Shooter/UI",
			"ShooterUE/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
