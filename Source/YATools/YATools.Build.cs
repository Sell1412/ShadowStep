using UnrealBuildTool;

public class YATools : ModuleRules
{
    public YATools(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", 
            "Engine",
            "InputCore",
            "CoreUObject", 
            "GameplayTags",
            "ShadowStep"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "UMG",
            "Slate",
            "UnrealEd",
            "SlateCore",
            "Blutility",
            "UMGEditor",
            "ToolMenus",
            "AssetTools",
            "EditorWidgets",
            "PropertyEditor",
            "BlueprintGraph",
            "ScriptableEditorWidgets",
            "EditorScriptingUtilities",
        });
    }
}