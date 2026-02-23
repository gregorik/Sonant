// Copyright (c) 2025 GregOrigin. All Rights Reserved.

using UnrealBuildTool;

public class Sonant : ModuleRules
{
    public Sonant(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		CppStandard = CppStandardVersion.Cpp20; 
	//	bUseUnity = false; 

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "GameplayTags",
                "AudioModulation",
                "DeveloperSettings",
                "PhysicsCore"
            }
        );
    }
}