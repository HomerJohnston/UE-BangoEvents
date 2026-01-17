#pragma once
#include "BangoScripts/Triggers/BangoNamedClassBase.h"
#include "Templates/SubclassOf.h"
#include "Engine/EngineTypes.h"

#include "BangoTargetGroup.generated.h"

enum class EBangoTargetImplementedInBlueprint : uint8
{
	Undetermined,
	NotImplementedInBlueprint,
	ImplementedInBlueprint,
};

UENUM(BlueprintType)
enum class EBangoTargetQueryMobileType : uint8
{
	Unspecified,
	AllDynamicObjects,
	AllStaticObjects,
	AllObjects,
};

/**
 * Inherit from this class to create new target groups for triggers. Target groups contain some settings that will get passed over to the Target Locator for the trigger.
 */
UCLASS(Abstract)
class BANGOSCRIPTS_API UBangoTargetGroup : public UBangoNamedClassBase
{
	GENERATED_BODY()
	
	// ==========================================
	// CONSTRUCTOR

public:

	UBangoTargetGroup();
	
	// ==========================================
	// SETTINGS

protected:

	/** If this is specified, it will filter the available target actors coming from the Target Locator. 
	 * If this is not specified, Bango will expose a dropdown to select it on the trigger. If left unset, will allow all AActor subclasses. */
	UPROPERTY(EditDefaultsOnly, Category = "Bango")
	TSubclassOf<AActor> ActorType;
	
	/** If this is overridden, it will control how available target actors are filtered. By default, only allows movable actors, not static or stationary.
	 * You may want to change this if your trigger is being used to find stationary lights for a moving trigger, or similar. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bango")
	EBangoTargetQueryMobileType QueryType = EBangoTargetQueryMobileType::Unspecified;
	
	/** This is used by triggers to gather candidate actors in the trigger volume area. Defaults to ECC_WorldDynamic. */
	UPROPERTY(EditDefaultsOnly, Category = "Bango")
	TEnumAsByte<ECollisionChannel> QueryCollisionChannel;
	
	/** Normally, Bango will ignore any Target classes that have been subclassed. Look at UBangoTarget_Player for more commments. */
	UPROPERTY(EditDefaultsOnly, Category = "Bango")
	bool bUseEvenIfInherited = false;
		
	// ==========================================
	// PUBLIC API (C++)
	
public:
	
	/** This is used by triggers to find final active actors for the trigger. */
	virtual bool IsTarget(AActor* Candidate);
		
	// ==========================================
	// PUBLIC API (BLUEPRINT)
	
protected:
	
	/** This is used by triggers to find final active actors for the trigger. */
	UFUNCTION(BlueprintImplementableEvent)
	bool K2_IsTarget(AActor* Candidate);
	
	// ==========================================
	// INTERNAL USE
	
private:
	
	// Internal use only.
	bool __IsTarget(AActor* Candidate);

	// Internal use only.
	ECollisionChannel GetQueryCollisionChannel() const;

	// Internal use only.
	EBangoTargetImplementedInBlueprint ImplementedInBlueprint = EBangoTargetImplementedInBlueprint::Undetermined;
};