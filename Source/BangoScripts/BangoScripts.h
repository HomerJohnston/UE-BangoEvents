// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"

class FBangoScriptsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	void StartupModule() override;
	void ShutdownModule() override;
};
