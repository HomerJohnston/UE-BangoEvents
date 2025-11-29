#pragma once
#include "BangoTargetCollectorQueryParams.h"
#include "Bango/Triggers/BangoNamedClassBase.h"

#include "BangoTargetCollector.generated.h"

/**
 * Target collectors are responsible for the initial task of gathering candidate actors. Typically these are using physics queries, or overlaps, to discover potential actors.
 * 
 * It is possible to create custom locators.
 */
UCLASS(Abstract)
class BANGO_API UBangoTargetCollector : public UBangoNamedClassBase
{
	GENERATED_BODY()
	
protected:
	/** Perform any required setup here. */
	virtual void Setup() { };
	
	/** Populate CachedResult with actors to return in this function. */
	virtual void UpdateCandidateActors(FBangoTargetCollectorQueryParams Params) { };
	
public:
	/** Give the Target request back any candidate actors we found. */
	TSet<AActor*>& GetCurrentCandidateActors(FBangoTargetCollectorQueryParams Params)
	{
		UpdateCandidateActors(Params);
		
		return CachedResult;
	}

protected:
	// TODO not sure what I'm trying to do here. I want to avoid risk of copying a massive array of actors around a lot. Keep one big array here and offer out a const reference to it?
	UPROPERTY(Transient)
	TSet<AActor*> CachedResult;
};