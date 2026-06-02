using UnrealBuildTool;
using System.Collections.Generic;

public class DungeonBroodTarget : TargetRules
{
	public DungeonBroodTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("DungeonBrood");
	}
}
