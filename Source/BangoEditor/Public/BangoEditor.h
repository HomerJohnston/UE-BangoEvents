// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSlateStyleSet;

class FBangoEditorModule : public IModuleInterface
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
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

	void RegisterMenus() const;
	
};

DECLARE_LOG_CATEGORY_EXTERN(BangoEditor, Log, All);