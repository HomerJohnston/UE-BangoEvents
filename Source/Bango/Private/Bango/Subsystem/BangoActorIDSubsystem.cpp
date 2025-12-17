#include "Bango/Subsystem/BangoActorIDSubsystem.h"

#include "Bango/Utility/BangoLog.h"

UBangoActorIDSubsystem* UBangoActorIDSubsystem::Get(UObject* WorldContext)
{
	UWorld* World = WorldContext->GetWorld();
	
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

void UBangoActorIDSubsystem::RegisterActor(UObject* WorldContextObject, FName ID, AActor* Actor)
{
	UBangoActorIDSubsystem* Subsystem = Get(WorldContextObject);
	
	if (!Subsystem)
	{
		return;
	}
	
	TWeakObjectPtr<AActor>* ExistingActor = Subsystem->Actors.Find(ID);

	if (ExistingActor)
	{
		if (ExistingActor->Get() != Actor)
		{
			UE_LOG(LogBango, Error, TEXT("Attempted to register ActorID %s but it was already registered with another actor!"), *ID.ToString());
		}
		else
		{
			UE_LOG(LogBango, Warning, TEXT("Attempted to register ActorID %s but it was already registered!"), *ID.ToString());
		}
	}
	else
	{
		Subsystem->Actors.Add(ID, Actor);
	}
}

void UBangoActorIDSubsystem::UnregisterActor(UObject* WorldContextObject, FName ID)
{
	UBangoActorIDSubsystem* Subsystem = Get(WorldContextObject);
	
	Subsystem->Actors.Remove(ID);
}

AActor* UBangoActorIDSubsystem::GetActor(UObject* WorldContextObject, FName ID)
{
	UBangoActorIDSubsystem* Subsystem = Get(WorldContextObject);
	
	TWeakObjectPtr<AActor>* ActorPtr = Subsystem->Actors.Find(ID);

	if (!ActorPtr)
	{
		// TODO ERROR LOGGING
		return nullptr;
	}
	
	if (!ActorPtr->IsValid())
	{
		Subsystem->Actors.Remove(ID);
		return nullptr;
	}

	bool bEvenIfPendingKill = false;
	return ActorPtr->Get(bEvenIfPendingKill);
}

AActor* UBangoActorIDBlueprintFunctionLibrary::GetActor(UObject* WorldContextObject, FName ActorID)
{
	return UBangoActorIDSubsystem::GetActor(WorldContextObject, ActorID);
}
