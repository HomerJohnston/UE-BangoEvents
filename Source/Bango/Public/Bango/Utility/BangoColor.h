// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#pragma once

namespace BangoColor
{
	inline FLinearColor LightGrey			(0.50,	0.50,	0.50);
	inline FLinearColor DarkGrey			(0.02,	0.02,	0.02);

	inline FLinearColor Error				(1.00,	0.00,	1.00);

	inline FLinearColor Red					(1.20,	0.10,	0.10);
	inline FLinearColor RedBase				(0.80,	0.10,	0.10);
	inline FLinearColor OrangeBase			(0.70,	0.40,	0.00);
	inline FLinearColor YellowBase			(0.60,	0.60,	0.00);
	inline FLinearColor GreenBase			(0.20,	0.90,	0.10);
	inline FLinearColor Green				(0.10,	1.20,	0.10);
	inline FLinearColor BlueBase			(0.15,	0.25,	0.80);
	inline FLinearColor LightBlue			(0.45,	0.55,	1.20);

	//inline FLinearColor Orange			(0.95,	0.60,	0.07);
	inline FLinearColor Orange				(1.50,	0.50,	0.10);
}

namespace BangoColorOps
{
	
	static FLinearColor BrightenColor(FLinearColor C)
	{
		float M = 10.0f;
		float N = 0.30f;
		return FLinearColor(M * C.R + N, M * C.G + N, M * C.B + N);
	}

	static FLinearColor EnhanceColor(FLinearColor C)
	{
		float M = 1.5f;
		float N = -0.05f;
		return FLinearColor(M * C.R + N, M * C.G + N, M * C.B + N);
	}

	static FLinearColor LightDesatColor(FLinearColor C)
	{
		float M = 0.30f;
		float N = 0.60f;
		return FLinearColor(M * C.R + N, M * C.G + N, M * C.B + N);
	}

	static FLinearColor DarkDesatColor(FLinearColor C)
	{
		float M = 0.10f;
		float N = 0.02f;
		return FLinearColor(M * C.R + N, M * C.G + N, M * C.B + N);
	}

	static FLinearColor VeryDarkDesatColor(FLinearColor C)
	{
		float M = 0.05f;
		float N = 0.01f;
		return FLinearColor(M * C.R + N, M * C.G + N, M * C.B + N);
	}
}