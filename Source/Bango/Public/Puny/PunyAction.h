#pragma once

#include "Bango/Editor/BangoDebugTextEntry.h"

#include "PunyAction.generated.h"

struct FPunyEventSignal;
struct FPunySignal;
enum class EPunyEventSignalType : uint8;
class UPunyEventComponent;
class UPunyTrigger;

UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class BANGO_API UPunyAction : public UObject
{
	GENERATED_BODY()
	
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================

	// -------------------------------------------------------------------
	// Settings Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// STATE
	// ============================================================================================

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
	UFUNCTION(BlueprintNativeEvent)
	void HandleSignal(UPunyEvent* Event, FPunyEventSignal Signal);
	
protected:
	UWorld* GetWorld() const override;

	UFUNCTION(BlueprintCallable)
	UPunyEventComponent* GetEventComponent() const;

	UFUNCTION(BlueprintCallable)
	UPunyEvent* GetEvent() const;
	
	UFUNCTION(BlueprintCallable)
	AActor* GetActor() const;

// TODO check all files for proper WITH_EDITORONLY_DATA usage, compare .h and .cpp 
	// ============================================================================================
	// EDITOR_SETTINGS
	// ============================================================================================

#if WITH_EDITORONLY_DATA
private:
	/** Set to override the editor display name. */
	UPROPERTY(Category="Advanced", DisplayName="Display Name Override", EditAnywhere, meta=(EditCondition="bUseDisplayName"))
	FText DisplayName;

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle))
	bool bUseDisplayName = false;
#endif

	// -------------------------------------------------------------------
	// Editor Settings Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// EDITOR STATE
	// ============================================================================================

	// -------------------------------------------------------------------
	// Editor State Getters/Setters
	// -------------------------------------------------------------------

	// ============================================================================================
	// EDITOR METHODS
	// ============================================================================================
#if WITH_EDITOR
public:
	UFUNCTION(BlueprintNativeEvent)
	void DebugDraw(UCanvas* Canvas, APlayerController* Cont);

	UFUNCTION(BlueprintCallable)
	FText GetDisplayName() const;
	
	virtual void AppendDebugData(TArray<FBangoDebugTextEntry>& Data);
#endif
};
