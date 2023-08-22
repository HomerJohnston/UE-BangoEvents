// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "BangoEventSignal.h"
#include "BangoEventComponent.generated.h"

class UBangoPlungerComponent;
struct FBangoTriggerSignal;
class UBangoEvent;
class UBangoTrigger;
class UBangoAction;
struct FBangoSignal;
class FCanvasTextItem;
struct FBangoDebugTextEntry;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBangoEventTriggered, UBangoEventComponent*, Eventcomponent, EBangoEventSignalType, Signal, UObject*, Instigator);

UCLASS(meta=(BlueprintSpawnableComponent))
class BANGO_API UBangoEventComponent : public USceneComponent
{
	GENERATED_BODY()

	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UBangoEventComponent();

	// ============================================================================================
	// SETTINGS
	// ============================================================================================

#if WITH_EDITORONLY_DATA
protected:
	UPROPERTY(Category="Bango|Testing", EditInstanceOnly, Transient)
	TSoftObjectPtr<AActor> DebugInstigator;
#endif

protected:
	// TODO check ShowInnerProperties meta on 5.2
	/**  */
	UPROPERTY(Category="Bango", DisplayName="Type", EditAnywhere, meta=(ShowInnerProperties))
	TObjectPtr<UBangoEvent> Event;

	/**  */
	UPROPERTY(Category="Bango", EditAnywhere)
	TArray<TObjectPtr<UBangoTrigger>> Triggers;

	/**  */
	UPROPERTY(Category="Bango", EditAnywhere)
	TArray<TObjectPtr<UBangoAction>> Actions;

	/** Intended for debug purposes. If true, the event will never be active or usable in any way during gameplay. */
	UPROPERTY(Category="Bango|Advanced", EditAnywhere, meta=(DisplayPriority=-1))
	bool bDisable = false;
	
	/**  */
	UPROPERTY(Category="Bango|Advanced", EditAnywhere, meta=(DisplayPriority=-1))
	bool bStartFrozen = false;

	UPROPERTY(Category="Bango|Advanced", EditAnywhere, meta=(DisplayPriority=-1))
	bool bDoNotFreezeWhenExpired = false;

	/**  */
	UPROPERTY(Category="Bango|Advanced", EditAnywhere, meta=(DisplayPriority=-1))
	bool bDestroyWhenExpired = false;
	
	// -------------------------------------------------------------------
	// Settings Getters/Setters
	// -------------------------------------------------------------------
public:
	/**  */
	bool GetStartsFrozen() const;

	UBangoEvent* GetEvent() const;

	bool GetIsDisabled() const;
	
	// ============================================================================================
	// STATE
	// ============================================================================================
private:
	/**  */
	UPROPERTY(Category="Bango|Debug", DisplayName="Frozen", VisibleAnywhere, meta=(DisplayPriority=-1))
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
public:
	FOnBangoEventTriggered OnEventTriggeredDelegate;
	
	// ============================================================================================
	// METHODS
	// ============================================================================================
public:
	void SetFrozen(bool bNewFrozen, bool bForceSet = false);
	
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override; 

	void DestroyOnBeginPlay();

	UFUNCTION()
	void OnEventTriggered(UBangoEvent* TriggeredEvent, FBangoEventSignal Signal);

public:
	UFUNCTION()
	void OnEventExpired(UBangoEvent* InEvent);

	// ============================================================================================
	// EDITOR SETTINGS
	// ============================================================================================
	
#if WITH_EDITORONLY_DATA
protected:

	/** Set to override the editor display name. */
	UPROPERTY(Category="Bango|Advanced|Display", DisplayName="Display Name Override", EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bUseDisplayName", DisplayPriority = -2))
	FText DisplayName;

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle, DisplayPriority = -2))
	bool bUseDisplayName = false;
	
	static TCustomShowFlag<EShowFlagShippingValue::ForceDisabled> BangoEventsShowFlag;

	/** Set to override the editor display color. */
	UPROPERTY(Category="Bango|Advanced|Display", EditAnywhere, meta=(EditCondition="bUseCustomColor", DisplayPriority = -2))
	FLinearColor CustomColor = FColor::White;

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle, DisplayPriority = -2))
	bool bUseCustomColor = false;

	/** Set to add a custom mesh display above the event proxy. */
	UPROPERTY(Category="Bango|Advanced|Display", EditAnywhere, meta=(EditCondition="bUseDisplayMesh", DisplayPriority = -2))
	UStaticMesh* DisplayMesh = nullptr;

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle, DisplayPriority = -2))
	bool bUseDisplayMesh = false;

	/** Set to change the display scale of the mesh. */
	UPROPERTY(Category="Bango|Advanced|Display", EditAnywhere, meta=(EditCondition="bUseDisplayMesh", EditConditionHides, HideEditConditionToggle, UIMin = 0.1, UIMax = 10.0, Delta=0.1, DisplayPriority = -2))
	float DisplayMeshScale = 1.0;

	/** Set to change the elevation of the mesh. */
	UPROPERTY(Category="Bango|Advanced|Display", EditAnywhere, meta=(EditCondition="bUseDisplayMesh", EditConditionHides, HideEditConditionToggle, UIMin = -200, UIMax = 200, Delta=10, DisplayPriority = -2))
	float DisplayMeshOffset = 0.0;

	const float DisplayMeshOffsetBase = +100.0;
	const float DebugTextOffsetBase = +100.0;
	const float DebugTextOffsetSizeScaler = +50.0;
#endif

	// -------------------------------------------------------------------
	// Editor Settings Getters/Setters
	// -------------------------------------------------------------------

#if WITH_EDITOR
	FText GetDisplayName();
#endif

	// ============================================================================================
	// EDITOR STATE
	// ============================================================================================

#if WITH_EDITORONLY_DATA
protected:
	UPROPERTY(Transient)
	UBangoPlungerComponent* PlungerComponent;

	UPROPERTY(Transient)
	UStaticMeshComponent* DisplayMeshComponent;

	FDelegateHandle DebugDrawService_Editor;
	
	FDelegateHandle DebugDrawService_Game;

public:
	//FSimpleDelegate OnSettingsChange;
#endif

	// -------------------------------------------------------------------
	// Editor State Getters/Setters
	// -------------------------------------------------------------------

#if WITH_EDITOR
public:
	virtual FLinearColor GetDisplayColor() const;
#endif
	
	// ============================================================================================
	// EDITOR METHODS
	// ============================================================================================
	
#if WITH_EDITOR
	void OnRegister() override;

	void UnregisterDebugDraw(const bool PIE);
	
	void ReregisterDebugDraw(const bool PIE);
	
	void OnUnregister() override;
	
	bool CanEditChange(const FProperty* InProperty) const override;
	
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void DebugDrawEditor(UCanvas* Canvas, APlayerController* PlayerController) const;

	void DebugDrawGame(UCanvas* Canvas, APlayerController* PlayerController) const;

	void UpdatePlungerProxy();
	
protected:
	
	void UpdateDisplayMesh();

	bool GetDebugTextScreenLocation(UCanvas* Canvas, FVector& ScreenLocation, double& DistSqrd) const;

	FCanvasTextItem GetDebugHeaderText(const FVector& ScreenLocationCentre, double Distance) const;

	TArray<FCanvasTextItem> GetDebugFooterText(UCanvas* Canvas, const FVector& ScreenLocationCentre, TDelegate<TArray<FBangoDebugTextEntry>()> DataGetter, double Distance) const;
	
	virtual TArray<FBangoDebugTextEntry> GetDebugDataString_Editor() const;

	virtual TArray<FBangoDebugTextEntry> GetDebugDataString_Game() const;
	
	FVector GetPlungerWorldLocation() const;

	bool HasInvalidData() const;
#endif
	
};