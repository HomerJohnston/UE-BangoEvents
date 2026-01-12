using UnrealBuildTool;

public class BangoUncooked : ModuleRules
{
    public BangoUncooked(ReadOnlyTargetRules Target) : base(Target)
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
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "Bango",
                "BlueprintGraph",
                "UnrealEd",
                "KismetCompiler",
                "Kismet",
                "EditorSubsystem",
                "GraphEditor",
                "BangoEditorTooling"
            }
        );
    }
}