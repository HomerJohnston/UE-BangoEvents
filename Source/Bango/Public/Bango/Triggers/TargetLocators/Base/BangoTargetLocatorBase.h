#pragma once
#include "BangoTargetLocatorQueryParams.h"

#include "BangoTargetLocatorBase.generated.h"

UCLASS(Abstract)
class BANGO_API UBangoTargetLocatorBase : public UObject
{
	GENERATED_BODY()
	
protected:
	/** Perform any required setup here. */
	virtual void Setup() { };
	
	/** Populate CachedResult with actors to return in this function. */
	virtual void UpdateCandidateActors(FBangoTargetLocatorQueryParams Params) { };
	
public:
	/** Give the Target request back any candidate actors we found. */
	TArray<AActor*>& GetCurrentCandidateActors(FBangoTargetLocatorQueryParams Params)
	{
		UpdateCandidateActors(Params);
		
		return CachedResult;
	}
	
protected:
	// TODO not sure what I'm trying to do here. I want to avoid risk of copying a massive array of actors around a lot. Keep one big array here and offer out a const reference to it?
	UPROPERTY(Transient)
	TArray<AActor*> CachedResult;
};