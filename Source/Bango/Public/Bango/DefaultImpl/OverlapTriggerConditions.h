#pragma once

#include "Bango/Core/TriggerCondition.h"

#include "OverlapTriggerConditions.generated.h"

class UBangoInstigatorFilter;

UCLASS(DisplayName="On Begin Overlap")
class BANGO_API UBangoTriggerCondition_OnBeginOverlap : public UBangoTriggerCondition
{
	GENERATED_BODY()

protected:
	// SETTINGS --------------------------------
	UPROPERTY(EditAnywhere, Instanced)
	UBangoInstigatorFilter* ActorFilter;
	
public:
	void Enable_Implementation() override;

	void Disable_Implementation() override;

protected:
	UFUNCTION()
	void OnBeginOverlap(AActor* BangoEventActor, AActor* InstigatorActor);
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
