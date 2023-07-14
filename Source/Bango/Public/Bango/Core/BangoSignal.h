// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#include "BangoSignal.generated.h"

UENUM(BlueprintType)
enum class EBangoSignal : uint8
{
	None		= 0,
	Activate	= 1,
	Deactivate	= 2,
	MAX			UMETA(Hidden)
};

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
}