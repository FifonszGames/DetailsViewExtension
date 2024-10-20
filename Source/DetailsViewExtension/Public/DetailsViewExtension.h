// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FDetailsViewExtensionModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
