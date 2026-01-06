#include "Bango/Core/BangoScriptBlueprint.h"

#include "Bango/Components/BangoScriptComponent.h"
#include "Bango/Utility/BangoHelpers.h"
#include "Bango/Utility/BangoLog.h"
#include "UObject/SavePackage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BangoScriptBlueprint)

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
const TSoftObjectPtr<AActor> UBangoScriptBlueprint::GetActor() const
{
	return Actor;
}

void UBangoScriptBlueprint::SoftDelete()
{
	/*
	// Make a duplicate of this blueprint and throw it in the transient package
	UBangoScriptBlueprint* Copy = DuplicateObject(this, GetTransientPackage(), *ScriptGuid.ToString());
	
	// TODO keeping the script alive with references in it might cause nuisances when deleting those referenced objects, how can I improve this?
	// We'll remove this on level change (OnMapLoad)
	Copy->AddToRoot();
	Copy->DeletedName = this->GetName();
	
	this->ClearEditorReferences();
	
	FEditorDelegates::OnMapLoad.AddUObject(Copy, &ThisClass::OnMapLoad);
	FBangoEditorDelegates::OnBangoActorComponentUndoDelete.AddUObject(Copy, &ThisClass::OnBangoActorComponentUndoDelete);
	*/
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptBlueprint::StopListeningForUndelete()
{
	FEditorDelegates::OnMapLoad.RemoveAll(this);
	FBangoEditorDelegates::OnBangoActorComponentUndoDelete.RemoveAll(this);
	FCoreUObjectDelegates::OnObjectTransacted.RemoveAll(this);		
	
	//RemoveFromRoot();
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
	
	FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetMapPackageExtension()); // FPackageName::GetAssetPackageExtension());
	
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
UBangoScriptBlueprint* UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(const TSoftClassPtr<UBangoScript> InClass)
{
	UBangoScriptBlueprint* BP = NULL;
	
	if (InClass != NULL)
	{
		BP = Cast<UBangoScriptBlueprint>(InClass.LoadSynchronous()->ClassGeneratedBy);
	}
	
	return BP;
}
#endif

#if WITH_EDITOR
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
#endif

#if WITH_EDITOR
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
		//NameElements.Add(TEXT("(") + OuterScriptComponent->GetName() + TEXT(")"));
		NameElements = { OuterScriptComponent->GetName() };
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
		// TODO this gets called with Outer == BlueprintNodeSpawner, is this bad?
		checkNoEntry();
	}
		
	AutoName = FString::Join(NameElements, TEXT(" "));
	
	//FName SanitizedName = MakeUniqueObjectName(Outer, UBangoScriptBlueprint::StaticClass(), FName(AutoName));
	
	return AutoName;
}
#endif

#if WITH_EDITOR
FString UBangoScriptBlueprint::RetrieveDeletedName()
{
	FString Temp = DeletedName;
	DeletedName = "";
	
	return Temp;
}
#endif

#if WITH_EDITOR
bool UBangoScriptBlueprint::RestoreToPackage()
{
	return true;
}
#endif
