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
void UBangoScriptBlueprint::ListenForUndelete()
{	
	ListenForUndeleteHandle = FCoreUObjectDelegates::OnObjectTransacted.AddUObject(this, &ThisClass::OnUndelete);
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptBlueprint::StopListeningForUndelete()
{
	if (ListenForUndeleteHandle.IsValid())
	{
		FCoreUObjectDelegates::OnObjectTransacted.Remove(ListenForUndeleteHandle);		
	}
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
#endif

#if WITH_EDITOR
void UBangoScriptBlueprint::OnUndelete(UObject* Object, const class FTransactionObjectEvent& TransactionEvent)
{
	if (UBangoScriptComponent* ScriptComponent = Cast<UBangoScriptComponent>(GetValid(Object)))
	{
		if (ScriptComponent->GetScriptGuid() == ScriptGuid)
		{
			StopListeningForUndelete();
			ScriptGuid.Invalidate();
		
			FBangoEditorDelegates::OnScriptContainerCreated.Broadcast(ScriptComponent, &ScriptComponent->Script);
		}
	}
}
#endif

#if WITH_EDITOR
void UBangoScriptBlueprint::UpdateAutoName(UObject* Outer)
{
	Rename(*GetAutomaticName(Outer));
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
#endif
