#include "Bango/Triggers/Targets/Base/BangoTargetBase.h"

UBangoTargetBase::UBangoTargetBase()
{
	PresenceCollisionChannel = ECC_WorldDynamic;
}

bool UBangoTargetBase::IsTarget(AActor* Candidate)
{
	UE_LOG(LogBango, Error, TEXT("Unimplimented function IsMember in %s"), *StaticClass()->GetName());
	return true;
}

bool UBangoTargetBase::__IsTarget(AActor* Candidate)
{
	if (ImplementedInBlueprint == EBangoTargetImplementedInBlueprint::Undetermined)
	{
		bool bIsImplementedInBlueprint = StaticClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED_OneParam(ThisClass, K2_IsTarget, AActor*));
		
		ImplementedInBlueprint = bIsImplementedInBlueprint ? EBangoTargetImplementedInBlueprint::ImplementedInBlueprint : EBangoTargetImplementedInBlueprint::NotImplementedInBlueprint;
	}
	
	return ImplementedInBlueprint == EBangoTargetImplementedInBlueprint::ImplementedInBlueprint ? K2_IsTarget(Candidate) : IsTarget(Candidate);
}

ECollisionChannel UBangoTargetBase::GetPresenceCollisionChannel() const
{
	return PresenceCollisionChannel;
}
