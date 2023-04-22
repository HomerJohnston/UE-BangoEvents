#include "BangoEditor.h"

#define LOCTEXT_NAMESPACE "FBangoEditorModule"

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
}

void FBangoEditorModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FBangoEditorModule, BangoEditor)