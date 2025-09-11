using UnrealBuildTool;

public class OverClockEditor: ModuleRules
{
    public OverClockEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        //PublicIncludePaths.AddRange(new string[] { "OverClock" });

        PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "OverClock" });

        PublicDependencyModuleNames.AddRange(
            new string[] 
            {
                "Core",
                "CoreUObject",
                "Engine",
                "UnrealEd",
                "Slate",
                "SlateCore",
                "AssetTools",
                "DeveloperSettings",
                "Json",
                "JsonUtilities",
                "Blutility",
                "EditorSubsystem",
                "EditorWidgets",
                "Projects",
                "DesktopWidgets",
                "EditorScriptingUtilities",
                "ToolMenus",
                "AssetRegistry",
                "UMG",
                "OverClock"
            });
    }
}
