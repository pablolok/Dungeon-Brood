using UnrealBuildTool;
using System.Collections.Generic;

public class DungeonBroodTarget : TargetRules
{
	public DungeonBroodTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("DungeonBrood");
	}
}
