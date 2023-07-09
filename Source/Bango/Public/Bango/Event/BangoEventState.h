#pragma once

#if WITH_EDITOR

UENUM(BlueprintType)
enum class EBangoEventState : uint8
{
	NONE		= 0			UMETA(Hidden),
	Initialized = 1 << 0,
	Active		= 1 << 1, 
	Frozen		= 1 << 2,
	Expired		= 1 << 3
};

inline uint8 operator|(EBangoEventState Left, EBangoEventState Right)
{
	return (uint8)Left | (uint8)Right;
}

struct FBangoEventStateFlag
{
	uint8 Value = 0;

	void SetFlag(EBangoEventState Flag, bool NewValue)
	{
		if (NewValue)
		{
			SetFlag(Flag);
		}
		else
		{
			ClearFlag(Flag);
			ClearFlag(Flag);
		}
	}
	
	void SetFlag(EBangoEventState In)
	{
		Value |= (uint8)In;
	}

	void ClearFlag(EBangoEventState In)
	{
		Value &= ~(uint8)In;
	}

	bool HasFlag(EBangoEventState In) const
	{
		return (Value & (uint8)In) == (uint8)In;
	}

	bool HasFlag(uint8 In) const
	{
		return (Value & In);
	}
	
	void ToggleFlag(EBangoEventState In)
	{
		Value ^= (uint8)In;
	}
};

#endif