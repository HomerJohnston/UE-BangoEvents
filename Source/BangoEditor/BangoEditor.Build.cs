using UnrealBuildTool;

public class BangoEditor : ModuleRules
{
    public BangoEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "Kismet",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Bango",
                "BangoUncooked",

                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "DeveloperSettings",
                "Projects",
                "InputCore",
                "UnrealEd",
                "BlueprintGraph",
                "GraphEditor",
                "ToolMenus",
                "KismetWidgets",
                "DataValidation",
                "EditorSubsystem",
				"ContentBrowserData",
				"AssetDefinition",
				"ClassViewer",
            }
        );
    }
}