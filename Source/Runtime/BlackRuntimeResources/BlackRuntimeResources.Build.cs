// Copyright (c) Black Megacorp. All Rights Reserved.

using UnrealBuildTool;

public class BlackRuntimeResources : ModuleRules
{
	public BlackRuntimeResources(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine"
        });

		PrivateDependencyModuleNames.AddRange(new string[] {
            "BlackCommon"
        });
	}
}
