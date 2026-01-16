#pragma once

#include "Bango/Core/BangoScript.h"
#include "Engine/Blueprint.h"
#include "IO/PackageId.h"

#include "BangoScriptBlueprint.generated.h"

struct FCanLoadMap;

UCLASS()
class BANGO_API UBangoScriptBlueprint : public UBlueprint
{
	GENERATED_BODY()

#if WITH_EDITOR
	friend class UBangoLevelScriptsEditorSubsystem;
	friend struct FBangoScriptContainer;
	
public:
	UBangoScriptBlueprint();
	
protected:
	bool ShouldBeMarkedDirtyUponTransaction() const override;
	
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

	static UBangoScriptBlueprint* GetBangoScriptBlueprintFromClass(const TSoftClassPtr<UBangoScript> ScriptClass);
	
	void OnBangoActorComponentUndoDelete(FGuid Guid, UBangoScriptBlueprint*& FoundBlueprint);
	
	//void OnUndelete(UObject* Object, const class FTransactionObjectEvent& TransactionEvent);
	
	void UpdateAutoName(UObject* Outer);
	
	static FString GetAutomaticName(UObject* Outer);
	
	FString RetrieveDeletedName();
	
	
	bool RestoreToPackage();
#endif
	
#if WITH_EDITORONLY_DATA
protected:
	/** Every script will have a unique ID. This is used to help differentiate/ID scripts when a script owner is duplicated or deleted/restored via undo. */
	UPROPERTY(NonPIEDuplicateTransient, TextExportTransient)
	FGuid ScriptGuid;
	
	// We use a raw FString to avoid editor issues with UE thinking that the actor is referenced by this script. 
	/** Scripts *may* be tied to an actor. This is only used for blueprint editor niceties and has no effect on gameplay. */
	UPROPERTY(NonPIEDuplicateTransient, TextExportTransient)
	FString ActorReference;
	
	// These are assigned when a script is deleted (i.e. when something owning a script is deleted), and is used to restore the script back to an identical-to-original state.
	FString DeletedName; // The original name, e.g. ~BangoScript
	FSoftObjectPath DeletedPackagePath; // The original package path, e.g. /Game/__BangoScripts__/Level/ActorID/ScriptID/BangoScript.uasset
	FGuid DeletedPackagePersistentGuid; // UPackage Guid
	FPackageId DeletedPackageId; // UPackage ID
	bool bFileReadOnly; // Whether the original .uasset file was read-only // TODO I may not need this if I just rely on version control to restore a deleted file?
#endif

#if WITH_EDITOR
public:
	const TSoftObjectPtr<AActor> GetActor() const;
	
	const FGuid& GetScriptGuid();

	void SetScriptGuid(FGuid InGuid);
#endif
};