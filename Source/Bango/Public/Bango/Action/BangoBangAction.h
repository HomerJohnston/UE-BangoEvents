#pragma once

#include "Bango/Action/BangoAction.h"
#include "BangoBangAction.generated.h"

UCLASS(Abstract, Within="BangoBangEvent")
class BANGO_API UBangoBangAction : public UBangoAction//, public IBangoBangActionInterface
{
	GENERATED_BODY()
	
	// ============================================================================================
	// Settings
	// ============================================================================================
protected:
	/** When set, starting of action will be delayed by the specified length of time. */
	UPROPERTY(Category="Action Settings", EditAnywhere, meta=(EditCondition="bUseStartDelay", ClampMin = 0.0))
	double StartDelay = 0;

	UPROPERTY()
	bool bUseStartDelay = false;

	/** If set, prevents this action's OnStart function from running. */
	UPROPERTY(Category="Action Settings", EditAnywhere)
	bool bBlockFromStarting = false;
	
public:
	void Start(UObject* StartInstigator); // override;

protected:
	void StartDelayed();
};
