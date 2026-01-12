#pragma once

#include "GameFramework/Actor.h"
#include "Misc/EnumRange.h"

#include "BangoTrigger.generated.h"

class UBangoTargetCollector;
class UBangoTriggerCondition;
class UBangoTargetGroup;

#define LOCTEXT_NAMESPACE "Bango"

UENUM()
enum class EBangoTriggerType : uint8
{
	Once,
	Toggle,
	
	Count UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EBangoTriggerType, EBangoTriggerType::Count);

UENUM()
enum class EBangoTriggerConditionSetting : uint8
{
	Any,
	All,
	Complex,
	
	Count UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EBangoTriggerConditionSetting, EBangoTriggerConditionSetting::Count);

UCLASS()
class ABangoTrigger : public AActor
{
	GENERATED_BODY()
	
public:
	ABangoTrigger();
	
	void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;
	
	UPROPERTY(EditAnywhere)
	FName TriggerName;
	
	UPROPERTY(EditAnywhere)
	FString TriggerDescription;
	
	UPROPERTY(EditAnywhere)
	EBangoTriggerType TriggerType = EBangoTriggerType::Once;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "TriggerType == EBangoTriggerType::Toggle", EditConditionHides))
	int32 ActivationLimit = 0;
	
	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<UBangoTargetCollector> TargetCollector;
	
	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<UBangoTargetGroup> TargetGroup;
	
	UPROPERTY(EditAnywhere)
	EBangoTriggerConditionSetting ConditionSetting;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Conditions.Num() > 2", EditConditionHides))
	FString ComplexConditionStatement;

	UPROPERTY(EditAnywhere, Instanced)
	TArray<TObjectPtr<UBangoTriggerCondition>> Conditions;
};

#undef LOCTEXT_NAMESPACE