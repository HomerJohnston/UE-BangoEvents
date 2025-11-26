#include "BangoEditor/Customizations/Properties/BangoScriptHolderCustomization.h"

#include "DetailWidgetRow.h"
#include "Bango/Core/BangoScriptObject.h"
#include "BangoEditor/DevTesting/BangoPackageHelper.h"
#include "Kismet2/KismetEditorUtilities.h"

FBangoScriptHolderCustomization::FBangoScriptHolderCustomization()
{
	
}

TSharedRef<IPropertyTypeCustomization> FBangoScriptHolderCustomization::MakeInstance()
{
	return MakeShared<FBangoScriptHolderCustomization>();
}

void FBangoScriptHolderCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ScriptClassProperty = PropertyHandle;
	
	HeaderRow.ValueContent()
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(SButton)
			.Text(INVTEXT("Create Script"))
			.OnClicked(this, &FBangoScriptHolderCustomization::OnClicked_CreateScript)
		]
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Text_Lambda( [] ()
			{
				return INVTEXT("Test");
			})
		]
		+ SVerticalBox::Slot()
		[
			SNew(SButton)
			.Text(INVTEXT("Edit Script"))
			.OnClicked(this, &FBangoScriptHolderCustomization::OnClicked_EditScript)
		]
	];
}

void FBangoScriptHolderCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	
}

FReply FBangoScriptHolderCustomization::OnClicked_CreateScript()
{
	TArray<UPackage*> Packages;
	
	ScriptClassProperty->GetOuterPackages(Packages);
	
	const UClass* OuterBaseClass = ScriptClassProperty->GetOuterBaseClass();

	if (Packages.Num() != 1)
	{
		return FReply::Handled();
	}

	FString BPName;
	UPackage* LocalScriptPackage = MakeLocalScriptPackage(Packages[0], BPName);
	
	
	/*
	UBlueprint* Script = FKismetEditorUtilities::CreateBlueprint(UBangoScriptObject::StaticClass(), LocalScriptPackage, FName(BPName), BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
	Script->SetFlags(RF_Public);
	Script->Modify();
	Script->MarkPackageDirty();
	LocalScriptPackage->SetDirtyFlag(true);
	*/
	
	return FReply::Handled();
}

FReply FBangoScriptHolderCustomization::OnClicked_EditScript()
{
	UAssetEditorSubsystem* Subsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();

	if (Subsystem)
	{
		UBlueprint* ScriptBlueprint = UBlueprint::GetBlueprintFromClass(GetScriptClass());
		
		Subsystem->OpenEditorForAsset(ScriptBlueprint);
	}
	
	return FReply::Handled();
}

TSubclassOf<UBangoScriptObject> FBangoScriptHolderCustomization::GetScriptClass() const
{
	void* ScriptClass_void;
	
	ScriptClassProperty->GetValueData(ScriptClass_void);
	
	TSubclassOf<UBangoScriptObject> ScriptClass = reinterpret_cast<UClass*>(ScriptClass_void);
	
	return ScriptClass;
}

UPackage* FBangoScriptHolderCustomization::MakeLocalScriptPackage(UObject* Outer, FString& NewBPName)
{
	FGuid UniqueID = FGuid::NewGuid();
	
	// Usually an OFP package in __ExternalActors__
	const UPackage* OutermostPackage = Outer->IsA<UPackage>() ? CastChecked<UPackage>(Outer) : Outer->GetOutermostObject()->GetPackage();

	const FString RootPath = OutermostPackage->GetName();
	const FString ExternalObjectPackageName = FBangoPackageHelper::GetLocalScriptPackageName(RootPath);
	
	FString FolderShortName = FString("LocalBangoScript") + TEXT("_UID_") + UniqueID.ToString(EGuidFormats::UniqueObjectGuid);
	TStringBuilderWithBuffer<TCHAR, NAME_SIZE> GloballyUniqueObjectPath;
	GloballyUniqueObjectPath += OutermostPackage->GetPathName();
	GloballyUniqueObjectPath += TEXT(".");
	GloballyUniqueObjectPath += FolderShortName;
	
	return CreatePackage(*GloballyUniqueObjectPath);
}
