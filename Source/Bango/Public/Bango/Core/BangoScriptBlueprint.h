#pragma once

#include "Bango/Core/BangoScript.h"

#include "BangoScriptBlueprint.generated.h"

UCLASS()
class BANGO_API UBangoScriptBlueprint : public UBlueprint
{
	GENERATED_BODY()

	friend class UBangoEditorSubsystem;
	friend struct FBangoScriptContainer;
	
public:
	UBangoScriptBlueprint();
	
#if WITH_EDITORONLY_DATA
protected:
	/** Every script will have a unique ID. This is used to help differentiate between scripts e.g. when a script owner (which may be an actor component or a simple UObject) is duplicated. */
	UPROPERTY()
	FGuid ScriptGuid;
	
	/** Scripts *may* be tied to an actor. */
	UPROPERTY()
	TSoftObjectPtr<AActor> Actor;
	
	FDelegateHandle ListenForUndeleteHandle;
	
	FName OverriddenName;
	
	FString DeletedName;
	FSoftObjectPath DeletedPackagePath;
	
public:
	const TSoftObjectPtr<AActor> GetActor() const;
#endif

#if WITH_EDITOR
protected:
	// When a script holder is deleted, the actual script blueprint (this) isn't deleted, it gets moved to the transient package. 
	// If the user undos their delete, this lets Bango restore the blueprint back the way it was.
	void SoftDelete();
	
	void StopListeningForUndelete();

	/** Forces the blueprint script to save itself. */
	void ForceSave();

	void BeginDestroy() override;
	
	void SetGuid(FGuid InGuid);
	
public:
	
	void OnMapLoad(const FString& String, FCanLoadMap& CanLoadMap);
	
	static UBangoScriptBlueprint* GetBangoScriptBlueprintFromClass(const TSoftClassPtr<UBangoScript> InClass);
	
	void OnBangoActorComponentUndoDelete(FGuid Guid, UBangoScriptBlueprint*& FoundBlueprint);
	
	//void OnUndelete(UObject* Object, const class FTransactionObjectEvent& TransactionEvent);
	
	void UpdateAutoName(UObject* Outer);
	
	static FString GetAutomaticName(UObject* Outer);
	
	FString RetrieveDeletedName();
	
	bool RestoreToPackage();
#endif
};