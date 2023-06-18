#pragma once

#include "Bango/Core/BangoAction.h"

#include "BangoAction_DebugLog.generated.h"

class ALight;

UCLASS(DisplayName="Debug Log")
class UBangoAction_DebugLog : public UBangoAction
{
	GENERATED_BODY()
protected:
	/** Optional debug string to print */
	UPROPERTY(Category="Settings", EditAnywhere)
	FString ActivateMessage;
	
	/** Optional debug string to print */
	UPROPERTY(Category="Settings", EditAnywhere)
	FString DeactivateMessage;
	
public:
	void OnStart_Implementation() override;

	void OnStop_Implementation() override;

protected:
	FText GetEventName();
	
	FString GetInstigatorName();
};