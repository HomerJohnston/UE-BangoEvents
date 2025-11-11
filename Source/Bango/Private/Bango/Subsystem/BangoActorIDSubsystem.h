#pragma once
#include "Bango/Core/BangoScriptHandle.h"

#include "BangoActorIDSubsystem.generated.h"

struct FBangoScriptHandle;
class UBangoScriptObject;

UCLASS()
class UBangoActorIDSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	static UBangoActorIDSubsystem* Get(UObject* WorldContext);

	bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
	
protected:
	UPROPERTY()
	TMap<FName, TWeakObjectPtr<AActor>> Actors;
	
public:
	static void RegisterActor(UObject* WorldContextObject, FName ID, AActor* Actor);

	static void UnregisterActor(UObject* WorldContextObject, FName ID);

	static AActor* GetActor(UObject* WorldContextObject, FName ID);
};

UCLASS()
class UBangoActorIDBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Bango", DisplayName = "Get Actor by ID", meta = (WorldContext = "WorldContextObject"))
	static AActor* GetActor(UObject* WorldContextObject, FName ID);
};