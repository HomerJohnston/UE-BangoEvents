#pragma once

#include "Bango/Core/BangoAction.h"

#include "DebugLogAction.generated.h"

UCLASS(DisplayName="Debug Log")
class UBangoAction_DebugLog : public UBangoAction
{
	GENERATED_BODY()
protected:
	/** Optional debug string to print */
	UPROPERTY(Category="Action", EditAnywhere)
	FString StartText;
	
	/** Optional debug string to print */
	UPROPERTY(Category="Action", EditAnywhere)
	FString StopText;
	
public:
	void Start_Implementation() override;

	void Stop_Implementation() override;

protected:
	FString GetEventName();
	
	FString GetInstigatorName();
};