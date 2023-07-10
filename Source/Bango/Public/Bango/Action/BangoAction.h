#pragma once

#include "BangoAction.generated.h"

enum class EBangoSignal : uint8;
class ABangoEvent;
class UWorld;
class UCanvas;
class APlayerController;

UCLASS(Abstract, Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew)
class BANGO_API UBangoAction : public UObject
{
	GENERATED_BODY()

	// ============================================================================================
	// Settings
	// ============================================================================================
private:
	/** Set to override the editor display name. */
	UPROPERTY(Category="Action Settings", DisplayName="Display Name Override", EditAnywhere)
	FText DisplayName;

	// ------------------------------------------
	// Settings Getters and Setters
	// ------------------------------------------

	// ============================================================================================
	// STATE
	// ============================================================================================
public:
	UPROPERTY(BlueprintReadOnly, Transient)
	TObjectPtr<UObject> Instigator;
	
	UPROPERTY(BlueprintReadOnly, Transient)
	bool bRunning = false;
	
	UPROPERTY(Transient)
	FTimerHandle StartTimerHandle;
	
	UPROPERTY(Transient)
	FTimerHandle StopTimerHandle;
	
	// ------------------------------------------
	// State Getters and Setters
	// ------------------------------------------

	// ============================================================================================
	// API
	// ============================================================================================
public:

public:
	/** Display name used in the editor and debug printing. Override to replace/add additional text. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetDisplayName();

	UFUNCTION(BlueprintNativeEvent, DisplayName="Receive Event Signal")
	void ReceiveEventSignal(EBangoSignal Signal, UObject* SignalInstigator);
	
protected:
	UWorld* GetWorld() const override;

	UFUNCTION(BlueprintCallable)
	ABangoEvent* GetEvent() const;

	// ============================================================================================
	// EDITOR
	// ============================================================================================
#if WITH_EDITOR
public:
	UFUNCTION(BlueprintNativeEvent)
	void DebugDraw(UCanvas* Canvas, APlayerController* Cont);
#endif
};
