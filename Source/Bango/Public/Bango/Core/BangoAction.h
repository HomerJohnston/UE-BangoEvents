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
	
	UPROPERTY(Category="Bango Action Settings", EditAnywhere)
	bool bIgnoreEventDelay = false;

	UPROPERTY(Category="Bango Action Settings", EditAnywhere)
	double Delay = 0;

	// ------------------------------------------
	// Getters and Setters
	// ------------------------------------------
	
	// ============================================================================================
	// State
	// ============================================================================================
private:
	UPROPERTY(Transient)
	FTimerHandle DelayHandle;

public:
	UPROPERTY(BlueprintReadOnly, Transient)
	ABangoEvent* Event;

	UPROPERTY(BlueprintReadOnly, Transient)
	UObject* Instigator;
	
	// ------------------------------------------
	// Getters and Setters
	// ------------------------------------------

	// ============================================================================================
	// API
	// ============================================================================================
public:
	void StartInternal(ABangoEvent* EventActor, UObject* NewInstigator, double EventDelay);

	void StopInternal(double EventDelay);

private:
	void PerformStart();

	void PerformStop();
	
protected:
	/** Performs action logic. */
	UFUNCTION(BlueprintNativeEvent)
	void Start();

	UFUNCTION(BlueprintNativeEvent)
	void Stop();
	
	/** Display name used in the editor and debug printing */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetDisplayName();
};
