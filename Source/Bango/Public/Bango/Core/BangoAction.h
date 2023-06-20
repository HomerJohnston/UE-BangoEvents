#pragma once

#include "BangoAction.generated.h"

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
	UPROPERTY(Category="Settings", EditAnywhere)
	FText DisplayName;

	/** When set, start actions will be delayed by the specified length of time. */
	UPROPERTY(Category="Settings", AdvancedDisplay, EditAnywhere, meta=(EditCondition="bUseStartDelay", ClampMin = 0.0))
	double StartDelay = 0;

	/**  */
	UPROPERTY()
	bool bUseStartDelay = false;
	
	/** When set, start actions will be delayed by the specified length of time. */
	UPROPERTY(Category="Settings", AdvancedDisplay, EditAnywhere, meta=(EditCondition="bUseStopDelay", ClampMin = 0.0))
	double StopDelay = 0;

	/**  */
	UPROPERTY()
	bool bUseStopDelay = false;
	
	// ------------------------------------------
	// Getters and Setters
	// ------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	bool GetUseStartDelay();
	
	UFUNCTION(BlueprintCallable)
	bool GetUseStopDelay();

	UFUNCTION(BlueprintCallable)
	double GetStartDelay();

	UFUNCTION(BlueprintCallable)
	double GetStopDelay();
	
	// ============================================================================================
	// State
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
	// Getters and Setters
	// ------------------------------------------

	// ============================================================================================
	// API
	// ============================================================================================
public:
	void Start(ABangoEvent* EventActor, UObject* NewInstigator);

	void StartDelayed();
	
	void Stop();

	void StopDelayed();

protected:
	/** Performs action logic. You do not need to call Super implementation when overriding. */
	UFUNCTION(BlueprintNativeEvent)
	void OnStart();

	/** Performs action logic. You do not need to call Super implementation when overriding. */
	UFUNCTION(BlueprintNativeEvent)
	void OnStop();

public:
	/** Display name used in the editor and debug printing. Override to replace/add additional text. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetDisplayName();

protected:
	UWorld* GetWorld() const override;

	UFUNCTION(BlueprintCallable)
	ABangoEvent* GetEvent() const;

#if WITH_EDITOR
public:
	UFUNCTION(BlueprintNativeEvent)
	void DebugDraw(UCanvas* Canvas, APlayerController* Cont);
#endif
};
