// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FBangoModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	void StartupModule() override;
	void ShutdownModule() override;
};
