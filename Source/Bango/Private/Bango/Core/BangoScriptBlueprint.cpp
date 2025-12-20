#include "Bango/Core/BangoScriptBlueprint.h"

#include "ObjectTools.h"
#include "Bango/Components/BangoScriptComponent.h"
#include "Bango/Utility/BangoHelpers.h"
#include "Bango/Utility/BangoLog.h"
#include "UObject/SavePackage.h"

// ----------------------------------------------

UBangoScriptBlueprint::UBangoScriptBlueprint()
{
#if WITH_EDITORONLY_DATA
	bForceFullEditor = true;
	OverriddenName = NAME_None;
#endif
}

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptBlueprint::SoftDelete()
{	
	AddToRoot();
	
	DeletedName = GetName();
	
	Rename(*ScriptGuid.ToString(), GetTransientPackage(), REN_DontCreateRedirectors | REN_NonTransactional);
	
	ClearEditorReferences();
	
	FEditorDelegates::OnMapLoad.AddUObject(this, &ThisClass::OnMapLoad);
	FBangoEditorDelegates::OnBangoActorComponentUndoDelete.AddUObject(this, &ThisClass::OnBangoActorComponentUndoDelete);
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptBlueprint::StopListeningForUndelete()
{
	FEditorDelegates::OnMapLoad.RemoveAll(this);
	FBangoEditorDelegates::OnBangoActorComponentUndoDelete.RemoveAll(this);
	FCoreUObjectDelegates::OnObjectTransacted.RemoveAll(this);		
	
	RemoveFromRoot();
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptBlueprint::ForceSave()
{
	if (!MarkPackageDirty())
	{
		return;
	}
		
	UPackage* Package = GetOutermost(); 
	
	FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
	
	bool bSuccess = UPackage::SavePackage(Package, this, *PackageFileName, FSavePackageArgs());
	
	UE_LOG(LogBango, Display, TEXT("Saved script blueprint? %i"), (uint8)(bSuccess));
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptBlueprint::BeginDestroy()
{
	StopListeningForUndelete();
	Super::BeginDestroy();
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptBlueprint::SetGuid(FGuid InGuid)
{
	Modify();
	ScriptGuid = InGuid;
}

void UBangoScriptBlueprint::OnMapLoad(const FString& String, FCanLoadMap& CanLoadMap)
{
	RemoveFromRoot();
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
UBangoScriptBlueprint* UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(const UClass* InClass)
{
	UBangoScriptBlueprint* BP = NULL;
		
	if (InClass != NULL)
	{
		BP = Cast<UBangoScriptBlueprint>(InClass->ClassGeneratedBy);
	}
		
	return BP;
}

void UBangoScriptBlueprint::OnBangoActorComponentUndoDelete(FGuid Guid, UBangoScriptBlueprint*& FoundBlueprint)
{
	if (FoundBlueprint)
	{
		return;
	}
	
	if (ScriptGuid == Guid)
	{
		StopListeningForUndelete();
		//ScriptGuid.Invalidate();
		FoundBlueprint = this;
	}
}
#endif

#if WITH_EDITOR
void UBangoScriptBlueprint::UpdateAutoName(UObject* Outer)
{
	Rename(*GetAutomaticName(Outer), nullptr, REN_DontCreateRedirectors);
}

FString UBangoScriptBlueprint::GetAutomaticName(UObject* Outer)
{
	FString AutoName = "";
	
	TArray<FString> NameElements = { TEXT("Script") };

	// Different Outer cases:
	// 1) UBangoScriptComponent - call it "Script (COMPONENTNAME)" and  the editor subsystem will let people rename it by renaming the component
	// 2) UActorComponent - call it "Script (COMPONENTNAME)" and in the FBangoScriptContainerCustomization we will expose a manual renaming field
	// 3) AActor - call it "Script (ACTORLABEL)" and in the FBangoScriptContainerCustomization we will expose a manual renaming field
	// 4) Null outer - this should never happen
	
	if (auto* OuterScriptComponent = Cast<UBangoScriptComponent>(Outer))
	{
		NameElements.Add(TEXT("(") + OuterScriptComponent->GetName() + TEXT(")"));
	}
	else if (auto* OuterActorComponent = Cast<UActorComponent>(Outer))
	{
		NameElements.Add("("+ OuterActorComponent->GetName() + ")");
	}
	else if (auto* OuterActor = Cast<AActor>(Outer))
	{
		NameElements.Add(OuterActor->GetActorLabel());
	}
	else
	{
		checkNoEntry();
	}
		
	AutoName = FString::Join(NameElements, TEXT(" "));
	return AutoName;
}

FString UBangoScriptBlueprint::RetrieveDeletedName()
{
	FString Temp = DeletedName;
	DeletedName = "";
	
	return Temp;
}
#endif
