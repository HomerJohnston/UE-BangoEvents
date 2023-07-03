#include "BangoEditor.h"

#define LOCTEXT_NAMESPACE "FBangoEditorModule"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

TSharedPtr<FSlateStyleSet> FBangoEditorModule::StyleSet = nullptr;

void FBangoEditorModule::StartupModule()
{
	static const FName PropertyEditor("PropertyEditor");
	
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditor);
	
	{
		// TODO localization
		TSharedRef<FPropertySection> Section = PropertyEditorModule.FindOrCreateSection("Actor", "Bango", INVTEXT("Bango"));
		Section->AddCategory("Bango");
		Section->AddCategory("Bango Action");
	}
	
	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("Bango"));
	check(Plugin.IsValid());
	
	const FString ResourcesDir = FPaths::ConvertRelativePathToFull(FPaths::Combine(Plugin->GetBaseDir(), TEXT("Resources")));
	
	StyleSet = MakeShareable(new FSlateStyleSet("BangoEditorStyleSet"));
	StyleSet->SetContentRoot(ResourcesDir);
	StyleSet->Set("ClassThumbnail.BangoEvent", new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("ClassIcons/BangoEvent.png")), FVector2D(256, 256)));
	StyleSet->Set("ClassThumbnail.BangoAction", new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("ClassIcons/BangoAction.png")), FVector2D(256, 256)));
	StyleSet->Set("ClassThumbnail.BangoTrigger", new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("ClassIcons/BangoTrigger.png")), FVector2D(256, 256)));
	
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
}

void FBangoEditorModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FBangoEditorModule, BangoEditor)