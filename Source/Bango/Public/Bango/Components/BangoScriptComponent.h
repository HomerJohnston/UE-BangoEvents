#pragma once

#include "Bango/Core/BangoScriptContainer.h"

#include "BangoEditorTooling/BangoDebugDrawServiceBase.h"
#include "InputCoreTypes.h"

#include "BangoScriptComponent.generated.h"

class UBlueprint;
class UBangoScript;

// TODO think more whether I want this. I currently just set 'This' to the closest owner actor.
/*
UENUM(BlueprintType)
enum class EBangoScriptComponent_ThisArg : uint8
{
	OwnerActor,
	ScriptComponent,
};
*/

UCLASS(meta = (BlueprintSpawnableComponent), HideCategories = ("Navigation","Activation"))
class BANGO_API UBangoScriptComponent : public UActorComponent, public FBangoDebugDrawServiceBase
{
	GENERATED_BODY()
	
	friend class UBangoLevelScriptsEditorSubsystem;
	friend class UBangoScriptBlueprint;
	
public:
	UBangoScriptComponent();
	
	void PrintState(FString Msg) const;

	void BeginPlay() override;
	
#if WITH_EDITOR
public:
	void OnRegister() override;
	
	void OnUnregister() override;
	
	// This is only used to spawn script assets for level instance added components
	void OnComponentCreated() override;

	void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	
	// This is only used to spawn script assets for CDO spawned components (actor dragged into world) as well as for duplicating any actors or any instance components
	void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
	
	void PostApplyToComponent() override;
	
	void UnsetScript();
	
	//void OnRename();
#endif
	
protected:
	/** Use this to run the script automatically upon BeginPlay. */
	UPROPERTY(EditAnywhere, DisplayName = "Autoplay")
	bool bRunOnBeginPlay = false;
	
	/** Optional description text for editor usage only */
	UPROPERTY(EditAnywhere)
	FString Description;
	
	/** The actual script instance. */
	UPROPERTY(VisibleAnywhere)
	FBangoScriptContainer Script;

#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> IconTexture;
	
	UPROPERTY(Transient)
	FBangoScriptHandle RunningHandle;
	
	// During undo/redo, the script property is reset and can't be read. I stash it when a transaction begins in this non-serialized value so I can actually read it when the transaction finishes.
	FBangoScriptContainer __UNDO_Script;
	
	UPROPERTY(Transient)
	TObjectPtr<UBillboardComponent> Billboard;
#endif
	
public:
	/** Runs the script. */
	UFUNCTION(BlueprintCallable)
	void Run();
	
#if WITH_EDITOR
public:
	
	FGuid GetScriptGuid() const;
	
	UBangoScriptBlueprint* GetScriptBlueprint(bool bForceLoad = false) const;
	
	void SetScriptBlueprint(UBangoScriptBlueprint* Blueprint); 
	
	void OnScriptFinished(FBangoScriptHandle FinishedHandle);
	
	void DebugDrawEditor(UCanvas* Canvas, FVector ScreenLocation, float Alpha) const override;
	
	void DebugDrawGame(UCanvas* Canvas, FVector ScreenLocation, float Alpha) const override;

	virtual void PreEditUndo() override;

	virtual void PostEditUndo(TSharedPtr<ITransactionObjectAnnotation> TransactionAnnotation) override;
	
	//UFUNCTION()
	//void OnClickedBillboard(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
#endif
};
