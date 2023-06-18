#pragma once

#include "Bango/Core/BangoTrigger.h"

#include "BangoTrigger_ActorOverlap.generated.h"

class UBangoInstigatorFilter;

UCLASS(DisplayName="Actor Overlap")
class BANGO_API UBangoTrigger_ActorOverlap : public UBangoTrigger
{
	GENERATED_BODY()

protected:
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
	
	/** By default the event will use itself as the source of overlap triggers. Pick another actor to listen for overlap triggers from that actor instead. */
	UPROPERTY(DisplayName = "Overlap Actor", Category="Settings", EditAnywhere, meta=(EditCondition = "bUseTargetActor"))
	AActor* TargetActor;

	UPROPERTY()
	bool bUseTargetActor;
	
	/** Use this to determine which overlap events to use and which to ignore. */
	UPROPERTY(Category="Settings", EditAnywhere, Instanced)
	UBangoInstigatorFilter* InstigatorFilter;
	
	// ============================================================================================
	// STATE
	// ============================================================================================
protected:
	/***/
	TWeakObjectPtr<AActor> SubscribedActor = nullptr;
	
public:
	void Enable_Implementation() override;

	void Disable_Implementation() override;

public:
	/***/
	void SetTargetActor(AActor* NewTargetActor);
	
protected:
	UFUNCTION()
	void OnBeginOverlap(AActor* BangoEventActor, AActor* InstigatorActor);

	UFUNCTION()
	void OnEndOverlap(AActor* OverlapActor, AActor* InstigatorActor);
};
