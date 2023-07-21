#pragma once
#include "PunyEventSignal.h"
#include "PunyEventComponent.generated.h"

class UPunyPlungerComponent;
struct FPunyTriggerSignal;
class UPunyEvent;
class UPunyTrigger;
class UPunyAction;
struct FPunySignal;


//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPunyEventSignalLimitReached, UPunyEvent*, Event, EBangoSignal, Signal);

UCLASS(meta=(BlueprintSpawnableComponent))
class BANGO_API UPunyEventComponent : public UActorComponent
{
	GENERATED_BODY()

	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UPunyEventComponent();

	// ============================================================================================
	// SETTINGS
	// ============================================================================================

#if WITH_EDITORONLY_DATA
protected:
	/** Set to override the editor display name. */
	UPROPERTY(Category="Bango|Display", DisplayName="Display Name Override", EditInstanceOnly, BlueprintReadOnly, meta=(EditCondition="bUseDisplayName"))
	FText DisplayName;

	UPROPERTY()
	bool bUseDisplayName = false;
	
	/** Set to override the editor display color. */
	UPROPERTY(Category="Bango|Display", EditAnywhere, meta=(EditCondition="bUseCustomColor"))
	FLinearColor CustomColor = FColor::White;

	UPROPERTY()
	bool bUseCustomColor = false;

	/** Set to add a custom mesh display above the event proxy. */
	UPROPERTY(Category="Bango|Display", EditAnywhere, meta=(EditCondition="bUseDisplayMesh"))
	UStaticMesh* DisplayMesh = nullptr;

	UPROPERTY()
	bool bUseDisplayMesh = false;

	/** Set to change the display scale of the mesh. */
	UPROPERTY(Category="Bango|Display", EditAnywhere, meta=(EditCondition="bUseDisplayMesh", EditConditionHides, HideEditConditionToggle, UIMin = 0.1, UIMax = 10.0))
	float DisplayMeshScale = 1.0;

	/** Set to change the elevation of the mesh. */
	UPROPERTY(Category="Bango|Display", EditAnywhere, meta=(EditCondition="bUseDisplayMesh", EditConditionHides, HideEditConditionToggle, UIMin = -1000, UIMax = 1000))
	float DisplayMeshOffset = 0.0;

	const float DisplayMeshOffsetBase = +100.0;
	const float DebugTextOffsetBase = +100.0;
	const float DebugTextOffsetSizeScaler = +50.0;

#endif	
	// TODO check ShowInnerProperties meta on 5.2
	/**  */
	UPROPERTY(Category="Bango|Event", DisplayName="Event Type", EditAnywhere, meta=(ShowInnerProperties))
	UPunyEvent* Event;

	/**  */
	UPROPERTY(Category="Bango|Event", EditAnywhere)
	TArray<UPunyTrigger*> Triggers;

	/**  */
	UPROPERTY(Category="Bango|Event", EditAnywhere)
	TArray<UPunyAction*> Actions;

	/** Intended for debug purposes. If true, the event will never be active or usable in any way during gameplay. */
	UPROPERTY(Category="Bango|Advanced", EditAnywhere, meta=(DisplayPriority=-1))
	bool bDisable = false;
	
	/**  */
	UPROPERTY(Category="Bango|Advanced", EditAnywhere, meta=(DisplayPriority=-1))
	bool bStartFrozen = false;

	/**  */
	UPROPERTY(Category="Bango|Advanced", EditAnywhere, meta=(DisplayPriority=-1))
	bool bDoNotFreezeWhenExpired = false;

	/**  */
	UPROPERTY(Category="Bango|Advanced", EditAnywhere, meta=(DisplayPriority=-1))
	bool DestroyWhenExpired = false;
	
	// -------------------------------------------------------------------
	// Settings Getters/Setters
	// -------------------------------------------------------------------
public:
	/**  */
	bool GetStartsFrozen() const;
	
	// ============================================================================================
	// STATE
	// ============================================================================================
private:
	/**  */
	bool bIsFrozen = false;
	
	// -------------------------------------------------------------------
	// State Getters/Setters
	// -------------------------------------------------------------------
public:
	/**  */
	bool GetIsFrozen() const;
	
	// -------------------------------------------------------------------
	// Delegates/Events
	// -------------------------------------------------------------------

	// ============================================================================================
	// METHODS
	// ============================================================================================
public:
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override; 

	void DestroyOnBeginPlay();
	
public:
	FText GetDisplayName();

	void SetFrozen(bool bNewFrozen, bool bForceSet = false);

public:
	UFUNCTION()
	void OnEventExpired(UPunyEvent* InEvent);

#if WITH_EDITORONLY_DATA
	// ============================================================================================
	// EDITOR SETTINGS
	// ============================================================================================
protected:
	
	// -------------------------------------------------------------------
	// Editor Settings Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// EDITOR STATE
	// ============================================================================================

	UPROPERTY()
	UPunyPlungerComponent* Plunger;

	UPROPERTY()
	UStaticMeshComponent* DisplayMeshComponent;
	
	// -------------------------------------------------------------------
	// Editor State Getters/Setters
	// -------------------------------------------------------------------
public:
	virtual FLinearColor GetDisplayColor() const;
	
	// ============================================================================================
	// EDITOR METHODS
	// ============================================================================================

	bool CanEditChange(const FProperty* InProperty) const override;
	
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void UpdateDisplayMesh();
#endif
};