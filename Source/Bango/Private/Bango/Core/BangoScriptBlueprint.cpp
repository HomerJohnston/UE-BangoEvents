#include "Bango/Core/BangoScriptBlueprint.h"

#include "Bango/Components/BangoScriptComponent.h"
#include "Bango/Utility/BangoHelpers.h"
#include "Bango/Utility/BangoLog.h"
#include "UObject/SavePackage.h"

UBangoScriptBlueprint::UBangoScriptBlueprint()
{
	bForceFullEditor = true;
}

void UBangoScriptBlueprint::ListenForUndelete(FGuid InGuid)
{
	UE_LOG(LogBango, Display, TEXT("Listening for undelete..."));
	FCoreUObjectDelegates::OnObjectTransacted.AddUObject(this, &ThisClass::OnUndelete);
	
	Guid = InGuid;
}

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

void UBangoScriptBlueprint::OnUndelete(UObject* Object, const class FTransactionObjectEvent& TransactionEvent)
{
	UE_LOG(LogBango, Display, TEXT("Checking for undelete from %s"), *Object->GetName());
	
	if (UBangoScriptComponent* ScriptComponent = Cast<UBangoScriptComponent>(GetValid(Object)))
	{
		if (ScriptComponent->GetScriptGuid() == Guid)
		{
			UE_LOG(LogBango, Display, TEXT("We found our owner again! Let's reattach!"));
			FCoreUObjectDelegates::OnObjectTransacted.RemoveAll(this);
			Guid.Invalidate();
			
			FBangoEditorDelegates::OnScriptComponentCreated.Broadcast(ScriptComponent, this);
		}
	}
}

void UBangoScriptBlueprint::SetGuid(FGuid InGuid)
{
	Modify();
	Guid = InGuid;
}
