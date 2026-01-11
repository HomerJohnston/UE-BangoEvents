/*
#include "Bango/Utility/BangoEditorUtility.h"

FLinearColor Bango::Editor::Color::GetHashedColor(uint32 Hash, float Saturation, float Luminosity)
{
	static const TArray<FLinearColor> RainbowColors =
	{
		FLinearColor(0.90f, 0.20f, 0.20f), // Red
		FLinearColor(0.95f, 0.45f, 0.15f), // Orange
		FLinearColor(0.95f, 0.80f, 0.20f), // Yellow
		FLinearColor(0.55f, 0.85f, 0.25f), // Yellow-Green
		FLinearColor(0.25f, 0.80f, 0.45f), // Green
		FLinearColor(0.20f, 0.85f, 0.75f), // Green-Cyan
		FLinearColor(0.25f, 0.70f, 0.95f), // Cyan
		FLinearColor(0.25f, 0.45f, 0.95f), // Blue
		FLinearColor(0.40f, 0.35f, 0.90f), // Blue-Purple
		FLinearColor(0.65f, 0.35f, 0.85f), // Purple
		FLinearColor(0.85f, 0.35f, 0.65f), // Magenta
		FLinearColor(0.95f, 0.35f, 0.45f)  // Red-Magenta
	};	
		
	uint32 ColorIndex = Hash % RainbowColors.Num();
	
	static const FLinearColor DefaultColor = FLinearColor::Red;
	
	FLinearColor Color = RainbowColors.IsValidIndex(ColorIndex) ? RainbowColors[ColorIndex] : DefaultColor;
	
	Color = Desaturate(Color, 1.0f - Saturation);
	Color = Darken(Color, 1.0f - Luminosity);
	
	return Color;
}

FLinearColor Bango::Editor::Color::Desaturate(FLinearColor InColor, float Desaturation)
{
	Desaturation = FMath::Clamp(Desaturation, 0.0f, 1.0f);
	
	float Lum = InColor.GetLuminance();
	return FMath::Lerp(InColor, FLinearColor(Lum, Lum, Lum, InColor.A), Desaturation);
}

FLinearColor Bango::Editor::Color::Darken(FLinearColor InColor, float Darken)
{
	Darken = FMath::Clamp(1.0f - Darken, 0.0f, 1.0f);
	
	return FLinearColor(Darken * Darken * InColor.R, Darken * Darken * InColor.G, Darken * Darken * InColor.B, InColor.A);
}
*/