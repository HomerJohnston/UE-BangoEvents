// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BangoEditor/GPGEditorModuleBase.h"
#include "BangoEditor/Private/BangoEditor/ClassViewerFilter/BangoClassViewerFilter.h"
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
	
	static TSharedPtr<FBangoClassViewerFilter> BangoClassViewerFilter;
		
	// ============================================================================================
	// API
	// ============================================================================================
public:
	void StartupModule() override;

	void ShutdownModule() override;
	
protected:
	void LateRegisterClassFilter();
};

DECLARE_LOG_CATEGORY_EXTERN(BangoEditor, Log, All);