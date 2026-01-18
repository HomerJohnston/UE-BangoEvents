#include "BangoScripts/Core/BangoScriptContainer.h"

#include "BangoScripts/Core/BangoScript.h"
#include "BangoScripts/EditorTooling/BangoScriptsEditorLog.h"
#include "BangoScripts/Utility/BangoScriptsLog.h"
#include "Engine/AssetManager.h"
#include "Engine/Engine.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

// ----------------------------------------------

#if WITH_EDITOR
void FBangoScriptContainer::SetGuid(const FGuid& InGuid)
{
	check(!Guid.IsValid());
	
	Guid = InGuid;
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void FBangoScriptContainer::Unset()
{
	Guid.Invalidate();
	ScriptClass = nullptr;
	ScriptClass.Reset();
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void FBangoScriptContainer::SetScriptClass(TSubclassOf<UObject> NewScriptClass)
{
	ScriptClass = NewScriptClass;
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void FBangoScriptContainer::UpdateScriptInputs()
{
	TArray<FProperty*> Properties;
	
	if (GetScriptClass().IsValid())
	{
		TSubclassOf<UBangoScript> Script = GetScriptClass().Get();
		
		for (TFieldIterator<FProperty> PropertyIterator(Script); PropertyIterator; ++PropertyIterator)
		{
			FProperty* Property = *PropertyIterator;
			
			if (Property->HasAnyPropertyFlags(CPF_Edit | CPF_ExposeOnSpawn) && !Property->IsNative())
			{
				UE_LOG(LogBangoEditor, Display, TEXT("%s"), *Property->GetName());
				Properties.Add(Property);
			}
		}
	}
	
	int32 NumBagProperties = ScriptInputs.GetNumPropertiesInBag();
	
	const UPropertyBag* BagStruct = ScriptInputs.GetPropertyBagStruct();

	if (BagStruct)
	{
		TSet<FName> DeadProperties;
	
		for (int32 i = 0; i < NumBagProperties; ++i)
		{
			const FPropertyBagPropertyDesc* Desc = BagStruct->FindPropertyDescByIndex(i);
		
			if (Properties.ContainsByPredicate( [Desc] (const FProperty* Property) { return Desc->Name == Property->GetFName(); } ))
			{
				// We already have this property. We won't need to add it.
				Properties.RemoveAll([Desc] (const FProperty* Property) { return Desc->Name == Property->GetFName(); } );
			}
			else
			{
				// This property exists, but it's not supposed to. We need to remove it.
				DeadProperties.Add(Desc->Name);
			}
		}
		
		if (DeadProperties.Num() > 0)
		{
			//Modify();
			ScriptInputs.RemovePropertiesByName(DeadProperties.Array());
		}
	}
	
	for (FProperty* Property : Properties)
	{
		//Modify();
		ScriptInputs.AddProperty(Property->GetFName(), Property);
	}
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void FBangoScriptContainer::SetRequestedName(const FString& InName)
{
	RequestedName = InName;
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
const FString& FBangoScriptContainer::GetRequestedName() const
{
	return RequestedName;
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void FBangoScriptContainer::SetIsDuplicate()
{
	bIsDuplicate = true;
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
bool FBangoScriptContainer::ConsumeDuplicate()
{
	bool bWasDuplicate = bIsDuplicate;
	bIsDuplicate = false;
	return bWasDuplicate;
}
#endif

#if WITH_EDITOR
const FString& FBangoScriptContainer::GetDescription() const
{
	return Description;
}
#endif

// ----------------------------------------------
