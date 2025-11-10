#pragma once

#include "BangoScriptHandle.generated.h"

USTRUCT(BlueprintType)
struct FBangoScriptHandle
{
	GENERATED_BODY()

	FBangoScriptHandle();

private:
	UPROPERTY()
	FGuid Guid;

	/*
	UPROPERTY()
	TWeakObjectPtr<UBangoScriptObject> Script;
*/
	
	friend uint32 GetTypeHash(const FBangoScriptHandle& ScriptHandle)
	{
		return GetTypeHash(ScriptHandle.Guid);
	}

public:
	bool operator==(const FBangoScriptHandle& Other) const
	{
		return Guid == Other.Guid;
	}

	bool IsValid();
	
	void Invalidate();
};