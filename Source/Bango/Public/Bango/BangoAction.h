﻿// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/Editor/BangoDebugTextEntry.h"
#include "Core/BangoActionFunctionRecord.h"
#include "BangoAction.generated.h"

enum class EBangoEventSignalType : uint8;
class UBangoEventComponent;
class UBangoTrigger;
struct FBangoEventSignal;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnEventTrigger, UBangoEvent*, Event, UObject*, Instigator);

UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class BANGO_API UBangoAction : public UObject
{
	GENERATED_BODY()

#if WITH_EDITOR
	friend class FBangoActionPropertyCustomization;
#endif
	
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================

public:
	UBangoAction();
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================

protected:
	/**  */
	UPROPERTY(EditAnywhere, Category="Settings", meta=(DisplayPriority=-1))
	FName OnEventActivate;
	
	/**  */
	UPROPERTY(EditAnywhere, Category="Settings", meta=(DisplayPriority=-1))
	FName OnEventDeactivate;

#if WITH_EDITORONLY_DATA
protected:
	UPROPERTY(EditDefaultsOnly, Category="Settings", meta=(DisplayPriority=-1))
	TSet<FName> ActionFunctions;
#endif

	// -------------------------------------------------------------------
	// Settings Getters/Setters
	// -------------------------------------------------------------------
	
	// ============================================================================================
	// STATE
	// ============================================================================================

	UPROPERTY(Transient)
	FOnEventTrigger OnEventActivateDelegate;

	UPROPERTY(Transient)
	FOnEventTrigger OnEventDeactivateDelegate;
	
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
	void Initialize();
	
	UFUNCTION(BlueprintNativeEvent)
	void BeginPlay();
	
	UFUNCTION(BlueprintNativeEvent)
	void EndPlay(const EEndPlayReason::Type EndPlayReason);

	/** This function is subscribed to the Event's triggered delegate and is called by the event when it activates or deactivates. */
	UFUNCTION()
	void HandleSignal(UBangoEvent* Event, FBangoEventSignal Signal);

	void Handle(FName ActionFunction, UBangoEvent* Event, FBangoEventSignal Signal);

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

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
