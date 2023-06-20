#pragma once

#include "Bango/Core/BangoAction.h"

#include "BangoAction_ControlEvent.generated.h"

UENUM()
enum class EBangoAction_FreezeThawEvent_Type : uint8
{
	Thaw,
	Freeze,
	DoNothing,
};

UCLASS(DisplayName="Control Event")
class BANGO_API UBangoAction_ControlEvent : public UBangoAction
{
	GENERATED_BODY()
protected:
	UPROPERTY(Category="Settings", EditInstanceOnly)
	ABangoEvent* TargetEvent;

	UPROPERTY(Category="Settings", EditAnywhere)
	EBangoAction_FreezeThawEvent_Type OnStartAction = EBangoAction_FreezeThawEvent_Type::Thaw;

	UPROPERTY(Category="Settings", EditAnywhere)
	EBangoAction_FreezeThawEvent_Type OnStopAction = EBangoAction_FreezeThawEvent_Type::Freeze;
	
public:
	void OnStart_Implementation() override;

	void OnStop_Implementation() override;

private:
	void Execute(EBangoAction_FreezeThawEvent_Type Type);
};
