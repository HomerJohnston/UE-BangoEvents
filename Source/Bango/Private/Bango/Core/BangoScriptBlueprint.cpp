#include "Bango/Core/BangoScriptBlueprint.h"

#include "Bango/Components/BangoScriptComponent.h"
#include "Bango/Utility/BangoHelpers.h"
#include "Bango/Utility/BangoLog.h"
#include "UObject/SavePackage.h"

// ----------------------------------------------

#if WITH_EDITOR
UBangoScriptBlueprint::UBangoScriptBlueprint()
{
	bForceFullEditor = true;
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void UBangoScriptBlueprint::ListenForUndelete()
{	
	auto OnUndelete = [this] (UObject* Object, const class FTransactionObjectEvent& TransactionEvent)
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
	};
	
	ListenForUndeleteHandle = FCoreUObjectDelegates::OnObjectTransacted.AddLambda(OnUndelete);
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
