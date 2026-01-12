#pragma once

#include "LatentActions.h"
#include "Engine/LatentActionManager.h"

#define LOCTEXT_NAMESPACE "Bango"

class FBangoSleepAction : public FPendingLatentAction
{
public:
	float Duration;
	float TimeRemaining;
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;

	TMulticastDelegate<void()> OnComplete;
	TMulticastDelegate<void()> OnCancel;
	TMulticastDelegate<void()> OnSkip;
	TMulticastDelegate<void()> OnTick;

	bool bCancelled = false;
	bool bSkipped = false;
	bool bPaused = false;
    
	FBangoSleepAction(float InDuration, const FLatentActionInfo& LatentInfo)
		: Duration(InDuration)
		, TimeRemaining(InDuration)
		, ExecutionFunction(LatentInfo.ExecutionFunction)
		, OutputLink(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
	{
	}

	void UpdateOperation(FLatentResponse& Response) override;

	void NotifyActionAborted() override;

	void Cancel();

	void Skip();

	void SetPaused(bool bInPaused);

#if WITH_EDITOR
	// Returns a human readable description of the latent operation's current state
	FString GetDescription() const override;
#endif
};

#undef LOCTEXT_NAMESPACE
