#pragma once

#include "BangoScriptBlueprint.generated.h"

UCLASS()
class BANGO_API UBangoScriptBlueprint : public UBlueprint
{
	GENERATED_BODY()
	
public:
	UBangoScriptBlueprint();
	
public:
	void ListenForUndelete(FGuid InGuid);

	bool SupportsDelegates() const override { return false; }
	
	bool SupportedByDefaultBlueprintFactory() const override { return false; }
	
	void ForceSave();
	
protected:
	void OnUndelete(UObject* Object, const class FTransactionObjectEvent& TransactionEvent);
	
	UPROPERTY()
	FGuid Guid;
	
public:
	void SetGuid(FGuid InGuid);
};