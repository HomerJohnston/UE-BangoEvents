using UnrealBuildTool;

public class BangoUncooked : ModuleRules
{
    public BangoUncooked(ReadOnlyTargetRules Target) : base(Target)
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
                "BlueprintGraph",
                "UnrealEd",
                "KismetCompiler"
            }
        );
    }
}