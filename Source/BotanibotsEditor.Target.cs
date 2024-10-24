// Copyright © 2024 Botanibots Team. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class BotanibotsEditorTarget : TargetRules
{
	public BotanibotsEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new[]
		{
			"BotaniGame",
			"BotaniEditor",
			"BotaniUI",
			"BotaniAI",
			"BotaniCheats",
		});

		if (!bBuildAllModules)
		{
			NativePointerMemberBehaviorOverride = PointerMemberBehavior.Disallow;
		}
		
		BotanibotsGameTarget.ApplySharedBotaniTargetSettings(this);
	}
}
