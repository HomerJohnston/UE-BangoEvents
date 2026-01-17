#include "BangoScripts/Subsystem/BangoActorIDSubsystem.h"

#include "BangoScripts/Utility/BangoLog.h"
#include "Engine/World.h"

UBangoActorIDSubsystem* UBangoActorIDSubsystem::Get(UObject* WorldContext)
{
	check(WorldContext);
	
	UWorld* World = WorldContext->GetWorld();
	
	if (!World)
	{
		UE_LOG(LogBango, Error, TEXT("Failed to find world, this should never happen?"));
		return nullptr;
	}

	return World->GetSubsystem<UBangoActorIDSubsystem>();
}

void UBangoActorIDSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

bool UBangoActorIDSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UBangoActorIDSubsystem::RegisterActor(UObject* WorldContextObject, AActor* Actor, FName Name, FGuid Guid)
{
	UBangoActorIDSubsystem* Subsystem = Get(WorldContextObject);
	
	if (!Subsystem)
	{
		return;
	}
	
	TPair<TWeakObjectPtr<AActor>, FName>* Pair = Subsystem->ActorsByGuid.Find(Guid);

	if (Pair)
	{
		UE_LOG(LogBango, Warning, TEXT("Attempted to register Actor %s but actor was already registered!"), *Actor->GetName());
		return;
	}
	
	if (Name != NAME_None)
	{
		TPair<TWeakObjectPtr<AActor>, FGuid>* PairByName = Subsystem->ActorsByName.Find(Name);
		
		if (PairByName)
		{
			UE_LOG(LogBango, Warning, TEXT("Attempted to register Bango Name %s but it was already registered to another actor!"), *Name.ToString());
			return;
		}
		
		FGuidRegistration NewGuidReg { Actor, Guid };
		Subsystem->ActorsByName.Add(Name, NewGuidReg);
	}
	
	FNameRegistration NewNameReg { Actor, Name };
	Subsystem->ActorsByGuid.Add(Guid, NewNameReg);
}

void UBangoActorIDSubsystem::UnregisterActor(UObject* WorldContextObject, FGuid Guid)
{
	UBangoActorIDSubsystem* Subsystem = Get(WorldContextObject);
	check(Subsystem);
	
	if (!Guid.IsValid())
	{
		UE_LOG(LogBango, Warning, TEXT("Tried to unregister Bango Actor ID but Guid was null!"));
		return;
	}
	
#if 1
	FNameRegistration NameReg = Subsystem->ActorsByGuid.FindAndRemoveChecked(Guid);
	
	if (NameReg.Value != NAME_None)
	{
		Subsystem->ActorsByName.Remove(NameReg.Value);
	}
#endif

	// Safer variant for debugging. Should I enable this in editor builds always? With error logging?
#if 0
	FNameRegistration* NameReg = Subsystem->ActorsByGuid.Find(Guid);
	
	if (NameReg)
	{
		if (NameReg->Value != NAME_None)
		{
			Subsystem->ActorsByName.Remove(NameReg->Value);
		}
		
		Subsystem->ActorsByGuid.Remove(Guid);
	}
#endif
}

void UBangoActorIDSubsystem::UnregisterActor(UObject* WorldContextObject, FName Name)
{
	UBangoActorIDSubsystem* Subsystem = Get(WorldContextObject);
	check(Subsystem);
	
	FGuidRegistration GuidReg = Subsystem->ActorsByName.FindAndRemoveChecked(Name);
	Subsystem->ActorsByGuid.Remove(GuidReg.Value);
}

AActor* UBangoActorIDSubsystem::GetActor(UObject* WorldContextObject, FName Name)
{
	if (Name == NAME_None)
	{
		UE_LOG(LogBango, Warning, TEXT("Tried to find actor with BangoName: None!"));
		return nullptr;
	}
	
	UBangoActorIDSubsystem* Subsystem = Get(WorldContextObject);
	check(Subsystem);
	
	FGuidRegistration* Registration = Subsystem->ActorsByName.Find(Name);

	if (!Registration)
	{
		// TODO ERROR LOGGING
		return nullptr;
	}
	
	if (!Registration->Key.IsValid())
	{
		UnregisterActor(Subsystem, Name);
		return nullptr;
	}

	bool bEvenIfPendingKill = false;
	return Registration->Key.Get(bEvenIfPendingKill);
}

AActor* UBangoActorIDSubsystem::GetActor(UObject* WorldContextObject, FGuid Guid)
{
	UBangoActorIDSubsystem* Subsystem = Get(WorldContextObject);
	check(Subsystem);
	
	FNameRegistration* Registration = Subsystem->ActorsByGuid.Find(Guid);

	if (!Registration)
	{
		// TODO ERROR LOGGING
		return nullptr;
	}
	
	if (!Registration->Key.IsValid())
	{
		UnregisterActor(Subsystem, Guid);
		return nullptr;
	}

	bool bEvenIfPendingKill = false;
	return Registration->Key.Get(bEvenIfPendingKill);
}

AActor* UBangoActorIDBlueprintFunctionLibrary::K2_GetActorByName(UObject* WorldContextObject, FName Name)
{
	return UBangoActorIDSubsystem::GetActor(WorldContextObject, Name);
}

AActor* UBangoActorIDBlueprintFunctionLibrary::K2_GetActorByGuid(UObject* WorldContextObject, FGuid Guid)
{
	return UBangoActorIDSubsystem::GetActor(WorldContextObject, Guid);
}
