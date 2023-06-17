#pragma once

#include "Bango/Core/BangoTrigger.h"

#include "BangoTrigger_ActorOverlap.generated.h"

class UBangoInstigatorFilter;

UCLASS(DisplayName="Actor Overlap")
class BANGO_API UBangoTrigger_ActorOverlap : public UBangoTrigger
{
	GENERATED_BODY()

protected:
	// SETTINGS
	// ============================================================================================

	// TODO: It hsould be possible for the event self to act as the overlappable actor, or for any other actor, or any other set of actors

	UPROPERTY(Category="Settings", EditAnywhere)
	AActor* TargetActor;
	
	/** Use to filter/ignore triggers from different actors */
	UPROPERTY(Category="Settings", EditAnywhere, Instanced, meta=(EditCondition="TargetActor==nullptr", EditConditionHides))
	UBangoInstigatorFilter* ActorFilter;
	
	// STATE
	// ============================================================================================
	TWeakObjectPtr<AActor> SubscribedActor = nullptr;
	
public:
	void Enable_Implementation() override;

	void Disable_Implementation() override;

	void SetTargetActor(AActor* NewTargetActor);
	
protected:
	UFUNCTION()
	void OnBeginOverlap(AActor* BangoEventActor, AActor* InstigatorActor);

	UFUNCTION()
	void OnEndOverlap(AActor* OverlapActor, AActor* InstigatorActor);
};
