#pragma once

#include "BangoAction.generated.h"

class ABangoEvent;

UCLASS(Abstract, Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew)
class BANGO_API UBangoAction : public UObject
{
	GENERATED_BODY()

	// ============================================================================================
	// Settings
	// ============================================================================================
private:
	UPROPERTY(Category="Bango Action Settings", EditAnywhere)
	FText DisplayName;

	// ------------------------------------------
	// Getters and Setters
	// ------------------------------------------
	
	// ============================================================================================
	// State
	// ============================================================================================
public:
	UPROPERTY(BlueprintReadOnly, Transient)
	TObjectPtr<ABangoEvent> Event;

	UPROPERTY(BlueprintReadOnly, Transient)
	TObjectPtr<UObject> Instigator;
	
	// ------------------------------------------
	// Getters and Setters
	// ------------------------------------------

	// ============================================================================================
	// API
	// ============================================================================================
public:
	void Start(ABangoEvent* EventActor, UObject* NewInstigator);

	void Stop();

protected:
	/** Performs action logic. You do not need to call Super implementation when overriding. */
	UFUNCTION(BlueprintNativeEvent)
	void OnStart();

	/** Performs action logic. You do not need to call Super implementation when overriding. */
	UFUNCTION(BlueprintNativeEvent)
	void OnStop();
	
	/** Display name used in the editor and debug printing. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetDisplayName();
};
