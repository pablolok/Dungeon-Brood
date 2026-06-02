using UnrealBuildTool;
using System.Collections.Generic;

public class DungeonBroodEditorTarget : TargetRules
{
	public DungeonBroodEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("DungeonBrood");
	}
}
