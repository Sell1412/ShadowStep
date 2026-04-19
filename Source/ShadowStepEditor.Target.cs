// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ShadowStepEditorTarget : TargetRules
{
	public ShadowStepEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;

		ExtraModuleNames.AddRange( new string[] { "ShadowStep" } );
	}
}
