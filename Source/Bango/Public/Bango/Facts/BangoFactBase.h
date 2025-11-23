#pragma once

#include "BangoFactBase.generated.h"

// ----------------------------------------------

#define BANGO_GENERATED_BODY(TYPE)\
		FBangoFact_##TYPE(){}\
		FBangoFact_##TYPE(TYPE InValue) : Value(InValue) {}\
		virtual ~FBangoFact_##TYPE() {}\
		\
		const void* GetValue() const { return reinterpret_cast<const void*>(&Value); }\

// ----------------------------------------------

#define DEFINE_BANGO_FACT(TYPE)\
namespace Bango\
{\
	void Set(FName Name, TYPE NewValue, UObject* WorldContext = nullptr)\
	{\
		TInstancedStruct<FBangoFactBase> NewFact = TInstancedStruct<FBangoFactBase>::Make<FBangoFact_##TYPE>(NewValue);\
		UBangoFactsSubsystem::SetFact(Name, NewFact, WorldContext);\
	}\
	\
	bool GetValue(FName Name, TYPE& OutValue, UObject* WorldContext = nullptr)\
	{\
		TInstancedStruct<FBangoFactBase>* Fact = UBangoFactsSubsystem::GetFact(Name, WorldContext);\
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

USTRUCT()
struct FBangoFactBase
{
	GENERATED_BODY()

public:
	virtual const void* GetValue() const { return nullptr; };
};

// ----------------------------------------------
