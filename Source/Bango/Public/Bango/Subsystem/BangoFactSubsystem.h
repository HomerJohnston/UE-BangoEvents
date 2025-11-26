#pragma once
#include "Bango/Utility/BangoLog.h"
#include "StructUtils/InstancedStruct.h"
#include "StructUtils/PropertyBag.h"
#include "Bango/Facts/BangoFactBase.h"

#include "BangoFactSubsystem.generated.h"

UENUM()
enum class EBangoSetFactResult : uint8
{
	Undefined,
	Success,
	Failure,
};

USTRUCT()
struct FBangoFactSet
{
	GENERATED_BODY()
	
	UPROPERTY()
	TMap<FName, TInstancedStruct<FBangoFactBase>> Variables;
};

UCLASS()
class UBangoFactSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	/**
	 * All variables. Variables in the null ULevel bucket are global persistent variables (between levels). Variables in ULevel buckets will get destroyed between level changes.
	 * At any given time there would usually only be two entries in this map, one for game instance variables and one for level variables.
	 */
	UPROPERTY()
	TMap<ULevel*, FBangoFactSet> Variables;
	
protected:
	static UBangoFactSubsystem* GetSubsystem(UObject* WorldContext);
	
public:
	static TInstancedStruct<FBangoFactBase>* GetFact(FName Name, UObject* WorldContext);
	
	static EBangoSetFactResult SetFact(FName Name, TInstancedStruct<FBangoFactBase> NewValue, UObject* WorldContext = nullptr);
	
	static void DeleteFact(FName Name, UObject* WorldContext);
};
