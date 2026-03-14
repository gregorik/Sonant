// Copyright (c) 2026 GregOrigin. All Rights Reserved.

using UnrealBuildTool;

public class SonantCore : ModuleRules
{
    public SonantCore(ReadOnlyTargetRules Target) : base(Target)
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