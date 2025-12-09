#pragma once
#include "Bango/Core/BangoScriptContainer.h"

#include "BangoScriptComponent.generated.h"

class UBlueprint;
class UBangoScript;

UCLASS(meta = (BlueprintSpawnableComponent), HideCategories = ("Navigation","Activation"))
class BANGO_API UBangoScriptComponent : public UActorComponent
{
	GENERATED_BODY()
	
	friend class UBangoEditorSubsystem;
	
public:
	UBangoScriptComponent();
	
	void BeginPlay() override;
	
#if WITH_EDITOR
	
	void OnComponentCreated() override;

	void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	
	void PreSave(FObjectPreSaveContext SaveContext) override;
	
	void TrySetGUID();
	
	void UnsetScript();
#endif
	
protected:
	/** By default, scripts will begin at BeginPlay. Use this to disable the script from running. */
	UPROPERTY(EditAnywhere)
	bool bPreventStarting = false;
	
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
