#include "Bango/Subsystem/BangoFactsSubsystem.h"

#include "Kismet/GameplayStatics.h"

// ----------------------------------------------

UBangoFactsSubsystem* UBangoFactsSubsystem::GetSubsystem(UObject* WorldContext)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContext);
	
	if (GameInstance)
	{
		return GameInstance->GetSubsystem<UBangoFactsSubsystem>();
	}
	
	UE_LOG(LogBango, Error, TEXT("Failed to find BangoFactsSubsystem!"));
	
	return nullptr;
}
// ----------------------------------------------

EBangoSetFactResult UBangoFactsSubsystem::SetFact(FName Name, TInstancedStruct<FBangoFactBase> NewValue, UObject* WorldContext)
{
	if (!IsValid(WorldContext))
	{
		UE_LOG(LogBango, Error, TEXT("A valid world context is required!"));
		return EBangoSetFactResult::Failure;
	}
	
	UBangoFactsSubsystem* Subsystem = GetSubsystem(WorldContext);
	
	if (!Subsystem)
	{
		return EBangoSetFactResult::Failure;
	}
	
	UWorld* World = GEngine->GetCurrentPlayWorld(WorldContext->GetWorld());
	
	FBangoFactSet& VarSet = Subsystem->Variables.FindOrAdd(World ? World->PersistentLevel : nullptr);

	VarSet.Variables.Add(Name, NewValue);
	
	return EBangoSetFactResult::Success;
}


// ----------------------------------------------

TInstancedStruct<FBangoFactBase>* UBangoFactsSubsystem::GetFact(FName Name, UObject* WorldContext)
{
	if (!IsValid(WorldContext))
	{
		UE_LOG(LogBango, Error, TEXT("A valid world context is required!"));
		return nullptr;
	}
	
	UBangoFactsSubsystem* Subsystem = GetSubsystem(WorldContext);
	
	if (!Subsystem)
	{
		return nullptr;
	}
	
	UWorld* World = GEngine->GetCurrentPlayWorld(WorldContext->GetWorld());
	
	FBangoFactSet* VarSet = Subsystem->Variables.Find(World ? World->PersistentLevel : nullptr);
	
	if (VarSet)
	{
		TInstancedStruct<FBangoFactBase>* Var = VarSet->Variables.Find(Name);
		
		if (!Var)
		{
			UE_LOG(LogBango, VeryVerbose, TEXT("Variable {%s} does not exist in the %s fact map!"), *Name.ToString(), World ? *FString::Format(TEXT("level %s"), {World->PersistentLevel.GetName()}) : TEXT("global"));
		}
		
		return Var;
	}
	else
	{
		UE_LOG(LogBango, VeryVerbose, TEXT("Variable {%s} does not exist in the %s map!"), *Name.ToString(), World ? *FString::Format(TEXT("level %s"), {World->PersistentLevel.GetName()}) : TEXT("global"));
		
		return nullptr;
	}
}

// ----------------------------------------------
