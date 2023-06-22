#pragma once

#include "Bango/Core/BangoAction.h"

#include "BangoAction_SetEventStatus.generated.h"

UENUM()
enum class EBangoAction_FreezeThawEvent_Type : uint8
{
	Active,
	Frozen,
	NoChange,
};

UCLASS(DisplayName="Set Event Status")
class BANGO_API UBangoAction_SetEventStatus : public UBangoAction
{
	GENERATED_BODY()
protected:
	UPROPERTY(Category="Settings", EditInstanceOnly)
	ABangoEvent* TargetEvent;

	UPROPERTY(Category="Settings", EditAnywhere)
	EBangoAction_FreezeThawEvent_Type StatusToSetOnStart = EBangoAction_FreezeThawEvent_Type::Active;

	UPROPERTY(Category="Settings", EditAnywhere)
	EBangoAction_FreezeThawEvent_Type StatusToSetOnStop = EBangoAction_FreezeThawEvent_Type::Frozen;
	
public:
	void OnStart_Implementation() override;

	void OnStop_Implementation() override;

private:
	void Execute(EBangoAction_FreezeThawEvent_Type Type);
};
