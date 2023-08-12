// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/Editor/BangoDebugTextEntry.h"
#include "Bango/Core/EventSignal.h"
#include "Action.generated.h"

enum class EBangoEventSignalType : uint8;
class UBangoEventComponent;
class UBangoTrigger;

UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class BANGO_API UBangoAction : public UObject
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
	void HandleSignal(UBangoEvent* Event, FBangoEventSignal Signal);
	
protected:
	UWorld* GetWorld() const override;

	UFUNCTION(BlueprintCallable)
	UBangoEventComponent* GetEventComponent() const;

	UFUNCTION(BlueprintCallable)
	UBangoEvent* GetEvent() const;
	
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

	virtual bool HasValidSetup();
#endif
};
