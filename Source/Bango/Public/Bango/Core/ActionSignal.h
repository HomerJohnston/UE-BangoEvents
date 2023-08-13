// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "ActionSignal.generated.h"

UENUM(BlueprintType)
enum class EBangoActionSignalType : uint8
{
	None,
	StartAction,
	StopAction,
	MAX
};

USTRUCT(BlueprintType)
struct FBangoActionSignal
{
	GENERATED_BODY()

	FBangoActionSignal();

	FBangoActionSignal(EBangoActionSignalType InType, UObject* InInstigator);
	
	EBangoActionSignalType Type;

	UObject* Instigator;
};

/*
namespace BangoUtility
{
	namespace Signals
	{
		template<typename T>
		void FillMap(TMap<EBangoSignal, T>& Map, T DefaultValue)
		{
			Map =
			{
				{ EBangoSignal::Activate, DefaultValue },
				{ EBangoSignal::Deactivate, DefaultValue }
			};
		}

		inline EBangoSignal GetOpposite(EBangoSignal InSignal)
		{
			uint8 Val = static_cast<uint8>(InSignal); // Convert {1, 2} to {0, 1}

			return static_cast<EBangoSignal>((Val + 1) % 2); // Flip to {1, 0} and convert to {2, 1}
		}
	}
*/