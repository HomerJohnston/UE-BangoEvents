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
	UPROPERTY(Category="Bango|Display", EditAnywhere, meta=(EditCondition="bUseCustomMesh"))
	UStaticMesh* CustomMesh = nullptr;

	UPROPERTY()
	bool bUseCustomMesh = false;

	/** Set to change the display scale of the mesh. */
	UPROPERTY(Category="Bango|Display", EditAnywhere, meta=(EditCondition="bUseCustomMesh", EditConditionHides, HideEditConditionToggle, UIMin = 0.1, UIMax = 10.0))
	float CustomMeshScale = 1.0;

	/** Set to change the elevation of the mesh. */
	UPROPERTY(Category="Bango|Display", EditAnywhere, meta=(EditCondition="bUseCustomMesh", EditConditionHides, HideEditConditionToggle, UIMin = -1000, UIMax = 1000))
	float CustomMeshOffset = 0.0;

	const float CustomMeshOffsetBase = +100.0;
	const float DebugTextOffsetBase = +100.0;
	const float DebugTextOffsetSizeScaler = +50.0;
#endif
	
	// TODO check ShowInnerProperties meta on 5.2
	UPROPERTY(Category="Bango|Event", DisplayName="Event Type", EditAnywhere, meta=(ShowInnerProperties))
	UPunyEvent* Event;
	
	UPROPERTY(Category="Bango|Event", EditAnywhere)
	TArray<UPunyTrigger*> Triggers;
	
	UPROPERTY(Category="Bango|Event", EditAnywhere)
	TArray<UPunyAction*> Actions;

	// -------------------------------------------------------------------
	// Settings Getters/Setters
	// -------------------------------------------------------------------
public:
	/**  */
	 
	
	// ============================================================================================
	// STATE
	// ============================================================================================
private:	
	// -------------------------------------------------------------------
	// State Getters/Setters
	// -------------------------------------------------------------------

	// -------------------------------------------------------------------
	// Delegates/Events
	// -------------------------------------------------------------------

	// ============================================================================================
	// METHODS
	// ============================================================================================
public:
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override; 

public:
	FText GetDisplayName();

#if WITH_EDITORONLY_DATA
	// ============================================================================================
	// EDITOR SETTINGS
	// ============================================================================================
protected:
	void OnRegister() override;

	void OnUnregister() override;
	
	// -------------------------------------------------------------------
	// Editor Settings Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// EDITOR STATE
	// ============================================================================================

	UPROPERTY()
	UPunyPlungerComponent* Plunger;

	UPROPERTY()
	UStaticMeshComponent* OverrideDisplayMesh;
	
	// -------------------------------------------------------------------
	// Editor State Getters/Setters
	// -------------------------------------------------------------------
public:
	virtual FLinearColor GetDisplayColor() const;
	
	// ============================================================================================
	// EDITOR METHODS
	// ============================================================================================
#endif

	void PostLoadSubobjects(FObjectInstancingGraph* OuterInstanceGraph) override;

	void DestroyComponent(bool bPromoteChildren) override;

	void InitializeComponent() override;

	void UninitializeComponent() override;
};