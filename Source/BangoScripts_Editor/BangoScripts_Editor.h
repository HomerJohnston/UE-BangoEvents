// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "BangoScripts_Editor/GPGEditorModuleBase.h"
#include "Private/ClassViewerFilter/BangoClassViewerFilter.h"
#include "Modules/ModuleManager.h"

class FSlateStyleSet;

class FBangoScripts_EditorModule : public IModuleInterface, public FGPGEditorModuleBase
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