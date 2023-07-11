// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

#if WITH_EDITOR
struct FBangoDebugTextEntry
{
	FBangoDebugTextEntry(FString InTextL, FString InTextR, FLinearColor InColor = FLinearColor::White) : TextL(InTextL), TextR(InTextR), Color(InColor) { }
	
	FString TextL;
	FString TextR;
	FLinearColor Color;
};
#endif