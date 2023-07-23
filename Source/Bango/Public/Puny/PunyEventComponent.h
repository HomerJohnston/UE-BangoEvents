#pragma once
#include "PunyEventSignal.h"
#include "PunyEventComponent.generated.h"

class UPunyPlungerComponent;
struct FPunyTriggerSignal;
class UPunyEvent;
class UPunyTrigger;
class UPunyAction;
struct FPunySignal;
class FCanvasTextItem;
struct FBangoDebugTextEntry;

UCLASS(meta=(BlueprintSpawnableComponent))
class BANGO_API UPunyEventComponent : public USceneComponent
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
	// TODO check ShowInnerProperties meta on 5.2
	/**  */
	UPROPERTY(Category="Bango", DisplayName="Type", EditAnywhere, meta=(ShowInnerProperties))
	UPunyEvent* Event;

	/**  */
	UPROPERTY(Category="Bango", EditAnywhere)
	TArray<UPunyTrigger*> Triggers;

	/**  */
	UPROPERTY(Category="Bango", EditAnywhere)
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

	UPunyEvent* GetEvent() const;
	
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
	
	static TCustomShowFlag<EShowFlagShippingValue::ForceDisabled> PunyEventsShowFlag;

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
#endif

	// ============================================================================================
	// EDITOR STATE
	// ============================================================================================
	
#if WITH_EDITORONLY_DATA

	UPROPERTY(Transient)
	UPunyPlungerComponent* PlungerComponent;

	UPROPERTY(Transient)
	UStaticMeshComponent* DisplayMeshComponent;
	
	FDelegateHandle DebugDrawService_Editor;
	FDelegateHandle DebugDrawService_Game;
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