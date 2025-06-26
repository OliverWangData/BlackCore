// Copyright (c) Black Megacorp. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FBlackRuntimeResourcesModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};