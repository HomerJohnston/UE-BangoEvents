#pragma once
#include "Bango/Core/BangoScriptContainer.h"

#include "BangoScriptComponent.generated.h"

class UBlueprint;
class UBangoScript;

UENUM(BlueprintType)
enum class EBangoScriptComponent_ThisArg : uint8
{
	OwnerActor,
	ScriptComponent,
};

UCLASS(meta = (BlueprintSpawnableComponent), HideCategories = ("Navigation","Activation"))
class BANGO_API UBangoScriptComponent : public UActorComponent
{
	GENERATED_BODY()
	
	friend class UBangoEditorSubsystem;
	friend class UBangoScriptBlueprint;
	
public:
	UBangoScriptComponent();
	
	void OnRegister() override;
	
	void BeginPlay() override;
	
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
#if WITH_EDITOR
	void OnComponentCreated() override;

	void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	
	void PreSave(FObjectPreSaveContext SaveContext) override;
	
	void PostEditImport() override;
	
	void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
	
	void PostLoad() override;
	
	void PostLoadSubobjects(FObjectInstancingGraph* OuterInstanceGraph) override;
	
	void PostInitProperties() override;
	
	void PostApplyToComponent() override;
	
	void PostReloadConfig(class FProperty* PropertyThatWasLoaded) override;
	
	void UnsetScript();
	
	void OnRename();
#endif
	
protected:
	/** By default, scripts will begin at BeginPlay. Use this to disable the script from running. */
	UPROPERTY(EditAnywhere)
	bool bPreventStarting = false;
	
	/** What to pass in as 'This' arg, defaults to the owning actor. */
	UPROPERTY(EditAnywhere, DisplayName = "'This' Argument")
	EBangoScriptComponent_ThisArg ThisArg;
	
	/** The actual script instance. */
	UPROPERTY(EditAnywhere)
	FBangoScriptContainer Script;

#if WITH_EDITOR
public:
	
	FGuid GetScriptGuid() const;
	
	UBangoScriptBlueprint* GetScriptBlueprint() const;
	
	void SetScriptBlueprint(UBangoScriptBlueprint* Blueprint); 
	
	void PostEditUndo(TSharedPtr<ITransactionObjectAnnotation> TransactionAnnotation) override;
#endif
};
