// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "Bango/Editor/BangoDebugTextEntry.h"
#include "BangoAction.generated.h"

enum class EBangoEventSignalType : uint8;
class UBangoEventComponent;
class UBangoTrigger;
struct FBangoEventSignal;

UENUM(BlueprintType)
enum class EBangoActionRun : uint8
{
	DoNothing,
	ExecuteStart,
	ExecuteStop,
};

UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class BANGO_API UBangoAction : public UObject
{
	GENERATED_BODY()
	
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
	UPROPERTY(EditAnywhere, Category="Advanced", meta=(DisplayPriority=-1))
	EBangoActionRun WhenEventActivates;
	
	/**  */
	UPROPERTY(EditAnywhere, Category="Advanced", meta=(DisplayPriority=-1))
	EBangoActionRun WhenEventDeactivates;

#if WITH_EDITORONLY_DATA
protected:
	/**  */
	UPROPERTY(EditDefaultsOnly)
	FString DoNothingDescription;
	
	/**  */
	UPROPERTY(EditDefaultsOnly)
	FString StartDescription;

	/**  */
	UPROPERTY(EditDefaultsOnly)
	FString StopDescription;
#endif
	
	// -------------------------------------------------------------------
	// Settings Getters/Setters
	// -------------------------------------------------------------------

#if WITH_EDITORONLY_DATA
public:
	FString GetDoNothingDescription() { return DoNothingDescription; };
	
	FString GetStartDescription() { return StartDescription; };

	FString GetStopDescription() { return StopDescription; };
#endif
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
	UFUNCTION()
	void HandleSignal(UBangoEvent* Event, FBangoEventSignal Signal);

	void Handle(EBangoActionRun WhatToDo, UBangoEvent* Event, FBangoEventSignal Signal);
	
protected:
	UFUNCTION(BlueprintNativeEvent)
	void Start(UBangoEvent* Event, UObject* Instigator);
	
	UFUNCTION(BlueprintNativeEvent)
	void Stop(UBangoEvent* Event, UObject* Instigator);
	
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

	const FString& GetDescriptionFor(EBangoActionRun ActionRun) const;

	EBangoActionRun LookupSettingForDescription(TSharedPtr<FString> Description) const;
#endif
};
