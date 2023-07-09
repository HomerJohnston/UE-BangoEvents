#pragma once

namespace BangoColors
{
	inline FLinearColor LightGrey			(0.50,	0.50,	0.50);
	inline FLinearColor DarkGrey			(0.02,	0.02,	0.02);

	inline FLinearColor Error				(1.00,	0.00,	1.00);

	inline FLinearColor RedBase			(0.20,	0.00,	0.00);
	inline FLinearColor OrangeBase			(0.15,	0.05,	0.00);
	inline FLinearColor YellowBase			(0.10,	0.10,	0.00);
	inline FLinearColor GreenBase			(0.00,	0.20,	0.00);
	inline FLinearColor BlueBase			(0.00,	0.00,	0.20);
}

namespace BangoColorOps
{
	
	static FLinearColor BrightenColor(FLinearColor C)
	{
		float M = 18.0f;
		float N = 0.40f;
		return FLinearColor(M * C.R + N, M * C.G + N, M * C.B + N);
	}

	static FLinearColor EnhanceColor(FLinearColor C)
	{
		float M = 2.0f;
		float N = -0.05f;
		return FLinearColor(M * C.R + N, M * C.G + N, M * C.B + N);
	}

	static FLinearColor LightDesatColor(FLinearColor C)
	{
		float M = 0.40f;
		float N = 0.20f;
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