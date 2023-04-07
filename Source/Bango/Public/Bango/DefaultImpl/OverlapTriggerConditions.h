#pragma once

#include "Bango/Core/TriggerCondition.h"

#include "OverlapTriggerConditions.generated.h"

class UBangoInstigatorFilter;

UCLASS(DisplayName="On Begin Overlap")
class BANGO_API UBangoTriggerCondition_OnBeginOverlap : public UBangoTriggerCondition
{
	GENERATED_BODY()

protected:
	// SETTINGS
	// ============================================================================================
	UPROPERTY(EditAnywhere, Instanced)
	UBangoInstigatorFilter* ActorFilter;

	/** Actor we will listen for overlap events from, will listen to parent Event actor if left blank */
	UPROPERTY(EditAnywhere, DisplayName="Use Target Actor", meta=(EditCondition="bUseTargetActor"));
	AActor* TargetActor;

	UPROPERTY()
	bool bUseTargetActor;
	// STATE
	// ============================================================================================

	TWeakObjectPtr<AActor> SubscribedActor = nullptr;
	
public:
	void Enable_Implementation() override;

	void Disable_Implementation() override;

protected:
	UFUNCTION()
	void OnBeginOverlap(AActor* OverlapActor, AActor* InstigatorActor);
};


UCLASS(DisplayName="On End Overlap")
class BANGO_API UBangoTriggerCondition_OnEndOverlap : public UBangoTriggerCondition
{
	GENERATED_BODY()

protected:
	// SETTINGS --------------------------------
	
public:
	void Enable_Implementation() override;

	void Disable_Implementation() override;
	
protected:
	UFUNCTION()
	void OnEndOverlap(AActor* BangoEventActor, AActor* InstigatorActor);
};
