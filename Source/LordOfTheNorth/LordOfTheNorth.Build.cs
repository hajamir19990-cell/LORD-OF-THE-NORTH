using UnrealBuildTool;

public class LordOfTheNorth : ModuleRules
{
    public LordOfTheNorth(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput"
        });
    }
}
