// Copyright (c) Black Megacorp. All Rights Reserved.

using UnrealBuildTool;

public class BlackCommon : ModuleRules
{
	public BlackCommon(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core"
		});
	}
}
