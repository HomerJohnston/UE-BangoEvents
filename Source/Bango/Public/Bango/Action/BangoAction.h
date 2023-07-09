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
	/** Set to override the editor display name. */
	UPROPERTY(Category="Action Settings", DisplayName="Display Name Override", EditAnywhere)
	FText DisplayName;

	/** When set, starting of action will be delayed by the specified length of time. */
	UPROPERTY(Category="Action Settings", EditAnywhere, meta=(EditCondition="bUseStartDelay", ClampMin = 0.0))
	double StartDelay = 0;

	UPROPERTY()
	bool bUseStartDelay = false;

	/** If set, prevents this action's OnStart function from running. */
	UPROPERTY(Category="Action Settings", EditAnywhere)
	bool bBlockFromStarting = false;
	
	// ------------------------------------------
	// Settings Getters and Setters
	// ------------------------------------------
public:
	UFUNCTION(BlueprintCallable)
	bool GetUseStartDelay();

	UFUNCTION(BlueprintCallable)
	double GetStartDelay();
	
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
	void Start(UObject* StartInstigator);

	void StartDelayed();

	
protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnStart();

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


USTRUCT(BlueprintType)
struct FTestStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TestCat1")
	float SomeProperty1;
	
	UPROPERTY(EditAnywhere, Category="TestCat2")
	float SomeProperty2;
	
	UPROPERTY(EditAnywhere, Category="TestCat2")
	float SomeProperty3;
};