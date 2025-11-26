// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BangoEditor/GPGEditorModuleBase.h"
#include "Modules/ModuleManager.h"

class FSlateStyleSet;

class FBangoEditorModule : public IModuleInterface, public FGPGEditorModuleBase
{
	// ============================================================================================
	// SETTINGS
	// ============================================================================================

	// ============================================================================================
	// STATE
	// ============================================================================================
protected:
	
	static TSharedPtr<FSlateStyleSet> StyleSet;
	
	// ============================================================================================
	// API
	// ============================================================================================
public:
	void StartupModule() override;

	void ShutdownModule() override;
};

DECLARE_LOG_CATEGORY_EXTERN(BangoEditor, Log, All);