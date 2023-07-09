#pragma once

struct FBangoDebugTextEntry
{
	FBangoDebugTextEntry(FString InTextL, FString InTextR, FColor InColor = FColor::White) : TextL(InTextL), TextR(InTextR), Color(InColor) { }
	
	FString TextL;
	FString TextR;
	FColor Color;
};