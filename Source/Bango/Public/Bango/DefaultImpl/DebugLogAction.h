#pragma once

#include "Bango/Core/BangoAction.h"

#include "DebugLogAction.generated.h"

class ALight;

UCLASS(DisplayName="Debug Log")
class UBangoAction_DebugLog : public UBangoAction
{
	GENERATED_BODY()
protected:
	/** Optional debug string to print */
	UPROPERTY(Category="Debug Log Settings", EditAnywhere)
	FString StartText;
	
	/** Optional debug string to print */
	UPROPERTY(Category="Debug Log Settings", EditAnywhere)
	FString StopText;
	
public:
	void OnStart_Implementation() override;

	void OnStop_Implementation() override;

protected:
	FString GetEventName();
	
	FString GetInstigatorName();
};