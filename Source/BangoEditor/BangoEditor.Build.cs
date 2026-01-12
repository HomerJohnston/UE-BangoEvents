using UnrealBuildTool;

public class BangoEditor : ModuleRules
{
    public BangoEditor(ReadOnlyTargetRules Target) : base(Target)
    {
	    PCHUsage = ModuleRules.PCHUsageMode.NoPCHs;
		
	    bUseUnity = false;

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
				"ContentBrowser",
				"ContentBrowserData",
				"AssetDefinition",
				"ClassViewer",
				"TypedElementFramework",
				"AssetTools",
				"UMGEditor",
				"BangoEditorTooling"
            }
        );
    }
}