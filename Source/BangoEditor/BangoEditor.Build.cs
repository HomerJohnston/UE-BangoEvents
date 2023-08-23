using UnrealBuildTool;

public class BangoEditor : ModuleRules
{
    public BangoEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

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
                "DeveloperSettings",
                "Projects",

                "GraphEditor",
                "UMGEditor",
				"BlueprintGraph",
				"Kismet",
				"UnrealEd",
                
                "InputCore"
            }
        );
    }
}