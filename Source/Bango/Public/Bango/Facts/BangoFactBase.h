#pragma once

#include "BangoFactBase.generated.h"

// ----------------------------------------------

/**
* Sample custom fact:
* 
* USTRUCT()							<------ No specifiers required
* struct FBangoFact_EGameFaction	<------ Struct must be named like this!
* {
*	GENERATED_BODY()
*	BANGO_GENERATED_BODY()			<------ Must include BANGO_GENERATED_BODY() macro!
*
*	UPROPERTY()						<------ No specifiers required 
*	EGameFaction Value;				<------ Member property must be named Value!
* };
* 
* DEFINE_BANGO_FACT_GETSET(EGameFaction); <--- this creates C++ methods: Bango::Set(...), Bango::GetValue(...), Bango::GetRef(...)
*/


USTRUCT()
struct FBangoFactBase
{
	GENERATED_BODY()

public:
	virtual const void* GetValue() const
	{
		return nullptr;
	};
};

// ----------------------------------------------

#define BANGO_GENERATED_BODY(TYPE)\
		FBangoFact_##TYPE(){}\
		FBangoFact_##TYPE(TYPE InValue) : Value(InValue) {}\
		virtual ~FBangoFact_##TYPE() {}\
		\
		const void* GetValue() const { return reinterpret_cast<const void*>(&Value); }\

// ----------------------------------------------

// TODO should I remove inline and move to a CPP or not
#define DEFINE_BANGO_FACT_GETSET(TYPE)\
namespace Bango\
{\
	inline void Set(FName Name, TYPE NewValue, UObject* WorldContext = nullptr)\
	{\
		TInstancedStruct<FBangoFactBase> NewFact = TInstancedStruct<FBangoFactBase>::Make<FBangoFact_##TYPE>(NewValue);\
		UBangoFactSubsystem::SetFact(Name, NewFact, WorldContext);\
	}\
	\
	inline bool GetValue(FName Name, TYPE& OutValue, UObject* WorldContext = nullptr)\
	{\
		TInstancedStruct<FBangoFactBase>* Fact = UBangoFactSubsystem::GetFact(Name, WorldContext);\
		\
		const void* Val = Fact->Get<FBangoFactBase>().GetValue();\
		\
		if (Val)\
		{\
			OutValue = *reinterpret_cast<const TYPE*>(Val);\
			return true;\
		}\
		\
		return false;\
	}\
	\
	/* TODO GetRef for expensive types? Return ref or ptr to original? */ \
}\

// ----------------------------------------------
