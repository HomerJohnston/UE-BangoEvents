#pragma once
#include "Bango/Utility/BangoLog.h"

#include "BangoTargetBase.generated.h"

enum class EBangoTargetImplementedInBlueprint : uint8
{
	Undetermined,
	NotImplementedInBlueprint,
	ImplementedInBlueprint,
};

/**
 * Inherit from this class to create new target groups for triggers.
 */
UCLASS(Abstract)
class BANGO_API UBangoTargetBase : public UObject
{
	GENERATED_BODY()
	
	// ==========================================
	// CONSTRUCTOR
public:
	UBangoTargetBase();
	
	// ==========================================
	// SETTINGS
protected:
	/** This is used by triggers to gather candidate actors in the trigger volume area. Defaults to ECC_WorldDynamic. */
	UPROPERTY(EditDefaultsOnly, Category = "Bango")
	TEnumAsByte<ECollisionChannel> PresenceCollisionChannel;
	
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
	ECollisionChannel GetPresenceCollisionChannel() const;

	// Internal use only.
	EBangoTargetImplementedInBlueprint ImplementedInBlueprint = EBangoTargetImplementedInBlueprint::Undetermined;
};