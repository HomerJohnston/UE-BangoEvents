// Originally written by Kyle Wilcox (HoJo/Homer Johnston), Ghost Pepper Games Inc.
// This file is public domain.

// This is a drop-in header to assist with basic module definition, customization registration/deregistration, etc.
// It is intended to be copy/pasted in any editor module.
// For usage, see additional comments below.

#pragma once

#include "AssetToolsModule.h"
#include "AssetTypeCategories.h"
#include "Editor.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"

#if WITH_EDITOR
#include "PropertyEditorModule.h"
#endif

class UObject;
class UThumbnailRenderer;
class IAssetTypeActions;
class IAssetTools;
class FComponentVisualizer;

using UAssetClass = TSubclassOf<UObject>;
using UThumbnailClass = TSubclassOf<UThumbnailRenderer>;

#define REGISTER_ASSET_TYPE_ACTION(NAME) AssetTypeActions.Add(MakeShared<NAME>()) // This is OBSOLETE - use UAssetDefinition for your assets instead
#define REGISTER_DETAIL_CUSTOMIZATION(CLASSNAME, CUSTOMIZATIONNAME) DetailCustomizations.Append({{ CLASSNAME::StaticClass(), FOnGetDetailCustomizationInstance::CreateStatic(&CUSTOMIZATIONNAME::MakeInstance)}})
#define REGISTER_PROPERTY_CUSTOMIZATION(CLASSNAME, CUSTOMIZATIONNAME) PropertyCustomizations.Append({{ *CLASSNAME::StaticStruct(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&CUSTOMIZATIONNAME::MakeInstance) }});
#define REGISTER_THUMBNAIL_RENDERER(CLASSNAME, THUMBNAILRENDERERNAME) ClassThumbnailRenderers.Append({{ CLASSNAME::StaticClass(), THUMBNAILRENDERERNAME::StaticClass() }});
#define REGISTER_COMPONENT_VISUALIZER(CLASSNAME, VISUALIZERCLASSNAME) ComponentVisualizers.Add( { CLASSNAME::StaticClass()->GetFName(), MakeShared<VISUALIZERCLASSNAME>() } )

/*
 * Inherit your editor module class from this, and then register customizations per the example below. You must call StartupModuleBase() in your StartupModule() and ShutdownModuleBase() in your ShutdownModule().
 * Example:
 *
 *	StartupModule()
 *	{
 *		// Optionally set an asset category for your plugin --- you can use it via UYourModule::GetAssetCategory
 *		AssetCategory = { FName("Yap"), LOCTEXT("Yap", "Yap") };
 *
 *		// Setup editor customizations, assets, visualizers, etc.
 *		REGISTER_DETAIL_CUSTOMIZATION(UYapProjectSettings, FDetailCustomization_YapProjectSettings);
 *		REGISTER_PROPERTY_CUSTOMIZATION(FYapCharacterDefinition, FPropertyCustomization_YapCharacterDefinition);
 *		REGISTER_THUMBNAIL_RENDERER(UYapCharacterAsset, UYapCharacterThumbnailRenderer);
 * 
 * 		// You must call this manually, after setting up all registrations!
 *		StartupModuleBase(); <--- this actually registers everything and stashes the registrations
 *
 *		...
 *	}
 *
 *	ShutdownModule()
 *	{
 *		// You must call this manually!
 *		ShutdownModuleBase(); <--- this actually unregisters everything
 *
 *		... other shutdown code of your own ...
 *	}
*/
class FGPGEditorModuleBase
{
	// ============================================================================================
	// SETTINGS
	/* Assign info to any of these four variables in your StartupModule(), and then call StartupModuleBase(). Call ShutdownModuleBase() in your ShutdownModule(). */
protected:
	TPair<FName, FText> AssetCategory;
	TArray<TSharedRef<IAssetTypeActions>> AssetTypeActions;
	TArray<TPair<TSubclassOf<UObject>, const FOnGetDetailCustomizationInstance>> DetailCustomizations;
	TArray<TPair<const UScriptStruct&, const FOnGetPropertyTypeCustomizationInstance>> PropertyCustomizations;
	TArray<TPair<UAssetClass, UThumbnailClass>> ClassThumbnailRenderers;
	TArray<TPair<FName, TSharedPtr<FComponentVisualizer>>> ComponentVisualizers;

	// ============================================================================================
	// STATE
private:
	TArray<TSharedRef<IAssetTypeActions>> RegisteredAssetTypeActions;
	TSet<FName> RegisteredCustomClassLayouts;
	TSet<FName> RegisteredCustomStructLayouts;
	static inline EAssetTypeCategories::Type RegisteredAssetCategory;
	TArray<FName> RegisteredComponentVisualizerClassNames;	

public: 
    static EAssetTypeCategories::Type GetAssetCategory()
    {
	    return RegisteredAssetCategory;
    };
	
private:
	void InitializeAll()
	{
		if (AssetCategory.Key != NAME_None && !AssetCategory.Value.IsEmpty())
		{
			RegisteredAssetCategory = RegisterAssetCategory(AssetCategory.Key, AssetCategory.Value);
		}
	
		RegisterAssetTypeActions();
		RegisterDetailCustomizations();
		RegisterPropertyCustomizations();
		RegisterThumbnailRenderers();
		RegisterComponentVisualizers();
		
		if (FPropertyEditorModule* PropertyEditorModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
		{
			PropertyEditorModule->NotifyCustomizationModuleChanged();
		}
		
		FCoreDelegates::OnPostEngineInit.RemoveAll(this);
	}
	
protected:
	void StartupModuleBase()
	{
		if (!GEditor || !GUnrealEd)
		{
			FCoreDelegates::OnPostEngineInit.AddRaw(this, &FGPGEditorModuleBase::InitializeAll);
			return;
		}
		
		InitializeAll();
	}
	
	void ShutdownModuleBase()
	{
		UnregisterAssetTypeActions();
		UnregisterDetailCustomizations();
		UnregisterPropertyCustomizations();
		UnregisterThumbnailRenderers();
		UnregisterComponentVisualizers();
	}
	
	// ========================================== 
	// REGISTRATION OF CATEGORY
	
private:
	// ------------------------------------------
	EAssetTypeCategories::Type RegisterAssetCategory(FName Name, FText Text)
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
			return AssetTools.RegisterAdvancedAssetCategory(Name, Text);
		}

		return EAssetTypeCategories::None;
	}

	// ========================================== 
	// REGISTRATION OF TYPES
	
private:
	// ------------------------------------------
	void RegisterAssetTypeActions()
	{
		if (FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools"))
		{
			IAssetTools& AssetTools = AssetToolsModule->Get();

			for (TSharedRef<IAssetTypeActions> Action : AssetTypeActions)
			{
				RegisterAssetTypeAction(AssetTools, Action);
			}
		}
	}
	
	void RegisterAssetTypeAction(IAssetTools& AssetTools, const TSharedRef<IAssetTypeActions> Action)
	{
		AssetTools.RegisterAssetTypeActions(Action);
		RegisteredAssetTypeActions.Add(Action);
	}

	// ------------------------------------------
	void RegisterDetailCustomizations()
	{
		if (FPropertyEditorModule* PropertyEditorModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
		{
			for (TPair<TSubclassOf<UObject>, const FOnGetDetailCustomizationInstance>& Customization : DetailCustomizations)
			{
				RegisterCustomClassLayout(Customization.Key, Customization.Value);
			}
		
			PropertyEditorModule->NotifyCustomizationModuleChanged();
		}
	}

	// ------------------------------------------
	void RegisterCustomClassLayout(const TSubclassOf<UObject> Class, const FOnGetDetailCustomizationInstance DetailLayout)
	{
		if (!Class)
		{
			return;
		}

		if (FPropertyEditorModule* PropertyEditorModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
		{
			PropertyEditorModule->RegisterCustomClassLayout(Class->GetFName(), DetailLayout);
			RegisteredCustomClassLayouts.Add(Class->GetFName());
		}
	}
	
	// ------------------------------------------
	void RegisterPropertyCustomizations()
	{
		if (FPropertyEditorModule* PropertyEditorModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
		{
			for (TPair<const UScriptStruct&, const FOnGetPropertyTypeCustomizationInstance>& Customization : PropertyCustomizations)
			{
				RegisterCustomStructLayout(Customization.Key, Customization.Value);
			}
		
			PropertyEditorModule->NotifyCustomizationModuleChanged();
		}
	}

	// ------------------------------------------
	void RegisterCustomStructLayout(const UScriptStruct& Struct, const FOnGetPropertyTypeCustomizationInstance DetailLayout)
	{
		if (FPropertyEditorModule* PropertyEditorModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
		{
			PropertyEditorModule->RegisterCustomPropertyTypeLayout(Struct.GetFName(), DetailLayout);
			RegisteredCustomStructLayouts.Add(Struct.GetFName());
		}
	}

	// ------------------------------------------
	void RegisterThumbnailRenderers()
	{
		UThumbnailManager& ThumbnailManager = UThumbnailManager::Get();

		for (TPair<UAssetClass, UThumbnailClass>& Pair : ClassThumbnailRenderers)
		{
			ThumbnailManager.RegisterCustomRenderer(Pair.Key, Pair.Value);
		}
	}
	
	// ------------------------------------------
	void RegisterComponentVisualizers()
	{
		if(!GUnrealEd)
		{
			//checkNoEntry();
			return;
		}
		
		for (TPair<FName, TSharedPtr<FComponentVisualizer>>& ComponentVisualizer : ComponentVisualizers)
		{
			RegisteredComponentVisualizerClassNames.Add(ComponentVisualizer.Key);
			GUnrealEd->RegisterComponentVisualizer(ComponentVisualizer.Key, ComponentVisualizer.Value);
		}
	}
	
	// ========================================== 
	// DE-REGISTRATION OF TYPES
	
private:
	// ------------------------------------------
	void UnregisterAssetTypeActions()
	{
		if (FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools"))
		{
			IAssetTools& AssetTools = AssetToolsModule->Get();

			for (const TSharedRef<IAssetTypeActions>& Action : RegisteredAssetTypeActions)
			{
				AssetTools.UnregisterAssetTypeActions(Action);
			}
		}
	
		RegisteredAssetTypeActions.Empty();
	}

	// ------------------------------------------
	void UnregisterDetailCustomizations()
	{
		if (FPropertyEditorModule* PropertyEditorModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
		{
			for (auto It = RegisteredCustomClassLayouts.CreateConstIterator(); It; ++It)
			{
				if (It->IsValid())
				{
					PropertyEditorModule->UnregisterCustomClassLayout(*It);
				}
			}
		}
	}
	
	// ------------------------------------------
	void UnregisterPropertyCustomizations()
	{
		if (FPropertyEditorModule* PropertyEditorModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
		{
			for (auto It = RegisteredCustomStructLayouts.CreateConstIterator(); It; ++It)
			{
				if (It->IsValid())
				{
					PropertyEditorModule->UnregisterCustomPropertyTypeLayout(*It);
				}
			}
		}
	}
	
	// ------------------------------------------
	void UnregisterThumbnailRenderers()
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			UThumbnailManager& ThumbnailManager = UThumbnailManager::Get();

			for (TPair<UAssetClass, UThumbnailClass>& Pair : ClassThumbnailRenderers)
			{
				ThumbnailManager.RegisterCustomRenderer(Pair.Key, Pair.Value);
			}
		}
	}
	
	// ------------------------------------------
	void UnregisterComponentVisualizers()
	{
		if(!GUnrealEd)
		{
			//checkNoEntry();
			return;
		}
		
		for (FName Name : RegisteredComponentVisualizerClassNames)
		{
			GUnrealEd->UnregisterComponentVisualizer(Name);
		}
	}
};

