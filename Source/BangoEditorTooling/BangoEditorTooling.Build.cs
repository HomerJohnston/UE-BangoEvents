using UnrealBuildTool;

// This module contains shared info for all other modules, such as editor colors and editor functions.
public class BangoEditorTooling : ModuleRules
{
    public BangoEditorTooling(ReadOnlyTargetRules Target) : base(Target)
    {
	    PCHUsage = ModuleRules.PCHUsageMode.NoPCHs;
		
	    bUseUnity = false;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
				"Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
				"UnrealEd"
            }
        );
    }
}