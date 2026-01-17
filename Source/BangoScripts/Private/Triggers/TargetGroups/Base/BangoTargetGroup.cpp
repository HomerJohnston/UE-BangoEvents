#include "BangoScripts/Triggers/TargetGroups/Base/BangoTargetGroup.h"

#include "BangoScripts/Utility/BangoLog.h"
#include "Engine/EngineTypes.h"

UBangoTargetGroup::UBangoTargetGroup()
{
	QueryCollisionChannel = ECC_WorldDynamic;
}

bool UBangoTargetGroup::IsTarget(AActor* Candidate)
{
	UE_LOG(LogBango, Error, TEXT("Unimplemented function IsMember in %s"), *StaticClass()->GetName());
	return true;
}

bool UBangoTargetGroup::__IsTarget(AActor* Candidate)
{
	if (ImplementedInBlueprint == EBangoTargetImplementedInBlueprint::Undetermined)
	{
		bool bIsImplementedInBlueprint = StaticClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED_OneParam(ThisClass, K2_IsTarget, AActor*));
		
		ImplementedInBlueprint = bIsImplementedInBlueprint ? EBangoTargetImplementedInBlueprint::ImplementedInBlueprint : EBangoTargetImplementedInBlueprint::NotImplementedInBlueprint;
	}
	
	return ImplementedInBlueprint == EBangoTargetImplementedInBlueprint::ImplementedInBlueprint ? K2_IsTarget(Candidate) : IsTarget(Candidate);
}

ECollisionChannel UBangoTargetGroup::GetQueryCollisionChannel() const
{
	return QueryCollisionChannel;
}
