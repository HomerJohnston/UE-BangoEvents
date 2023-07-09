#pragma once

#include "BangoEvent.h"

#include "Bango/Core/BangoInterfaces.h"

#include "BangoBangEvent.generated.h"

class UBangoBangTrigger;
class UBangoBangAction;

UCLASS()
class BANGO_API ABangoBangEvent : public ABangoEvent
{
	GENERATED_BODY()
	// ============================================================================================
	// CONSTRUCTION
	// ============================================================================================
public:
	ABangoBangEvent();
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
	
	// ============================================================================================
	// STATE
	// ============================================================================================

	// ============================================================================================
	// API
	// ============================================================================================
public:
	virtual bool ProcessTriggerSignal(EBangoSignal Signal, UObject* NewInstigator) override;

protected:
	virtual bool HasInvalidData() const override;

protected:
	bool Activate(UObject* ActivateInstigator);

	void StartActions(UObject* StartInstigator);
	
public:
#if WITH_EDITOR
public:
	void UpdateProxyState() override;

	FLinearColor GetColorBase() const override;

	FLinearColor GetColorForProxy() const override;

	TArray<FBangoDebugTextEntry> GetDebugDataString_Game() const override;

	TArray<FBangoDebugTextEntry> GetDebugDataString_Editor() const override;
#endif
};