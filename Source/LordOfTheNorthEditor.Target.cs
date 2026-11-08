using UnrealBuildTool;
using System.Collections.Generic;

public class LordOfTheNorthEditorTarget : TargetRules
{
    public LordOfTheNorthEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("LordOfTheNorth");
    }
}
