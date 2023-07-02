#pragma once

#include "Bango/Core/BangoTrigger.h"

#include "BangoTrigger_ActorOverlap.generated.h"

class UBangoInstigatorFilter;

enum class EBangoTriggerInstigatorAction : uint8;

UCLASS(DisplayName="Actor Overlap")
class BANGO_API UBangoTrigger_ActorOverlap : public UBangoTrigger
{
	GENERATED_BODY()
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	UBangoTrigger_ActorOverlap();
	
protected:
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
	
	UPROPERTY()
	bool bUseTargetActor;
	
	/** By default the event will use itself as the source of overlap triggers. Pick another actor to listen for overlap triggers from that actor instead. */
	UPROPERTY(DisplayName = "Get Overlap Events From Other Actor", Category="Overlap Settings", EditAnywhere, meta=(EditCondition = "bUseTargetActor"))
	AActor* TargetActor;
	
	/** Use this to determine which overlap events to use and which to ignore. */
	UPROPERTY(Category="Overlap Settings", EditAnywhere, Instanced)
	UBangoInstigatorFilter* InstigatorFilter;

	/** How to trigger the event when an overlap begins. */
	UPROPERTY(Category="Overlap Settings", DisplayName="On Begin Overlap", EditAnywhere)
	EBangoTriggerInstigatorAction BeginOverlapAction;

	/** How to trigger the event when an overlap ends. */
	UPROPERTY(Category="Overlap Settings", DisplayName="On End Overlap", EditAnywhere)
	EBangoTriggerInstigatorAction EndOverlapAction;
	
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

	void Handle(AActor* OverlapActor, AActor* InstigatorActor, EBangoTriggerInstigatorAction Action);
};
