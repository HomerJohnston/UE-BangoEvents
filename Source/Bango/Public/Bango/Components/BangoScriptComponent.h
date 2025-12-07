#pragma once
#include "Bango/BangoScriptHolder.h"

#include "BangoScriptComponent.generated.h"

class UBlueprint;
class UBangoScriptInstance;

UCLASS(meta = (BlueprintSpawnableComponent), HideCategories = ("Navigation","Activation"))
class BANGO_API UBangoScriptComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UBangoScriptComponent();
	
	void BeginPlay() override;
	
#if WITH_EDITOR
	
	void OnComponentCreated() override;

	void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	
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
	
	UBlueprint* GetScriptBlueprint() const;
	
	void SetScriptBlueprint(UBlueprint* Blueprint); 
#endif
};
