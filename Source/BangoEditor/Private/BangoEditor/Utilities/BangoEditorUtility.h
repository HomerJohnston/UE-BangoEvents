#pragma once

class UBangoScriptComponent;
class UBangoScriptInstance;

namespace Bango::Editor
{
	AActor* GetActorOwner(TSharedPtr<IPropertyHandle> Property);
	
	UPackage* MakeScriptPackage(UBangoScriptComponent* Component, UObject* Outer, FString& NewBPName);

	UPackage* MakeScriptPackage(TSharedPtr<IPropertyHandle> ScriptProperty, UObject* Outer, FString& NewBPName);
	
	UPackage* MakeScriptPackage_Internal(AActor* Actor, UObject* Outer, FString& NewBPName);
	
	UBlueprint* MakeScriptAsset(UPackage* InPackage, FString Name, FGuid Guid);
	
	bool SaveScriptPackage(UPackage* ScriptPackage, UBlueprint* ScriptBlueprint);
	
	void SoftDeleteScriptPackage(TSubclassOf<UBangoScriptInstance> ScriptClass);
}
