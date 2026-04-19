// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ShadowStepTarget : TargetRules
{
	public ShadowStepTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;

		ExtraModuleNames.AddRange( new string[] { "ShadowStep" } );
	}
}
