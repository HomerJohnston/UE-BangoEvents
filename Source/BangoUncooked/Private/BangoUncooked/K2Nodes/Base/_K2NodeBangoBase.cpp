#include "BangoUncooked/K2Nodes/Base/_K2NodeBangoBase.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

void UK2NodeBangoBase::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* Action = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(Action))
	{
		UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(Action);
		check(Spawner);

		ActionRegistrar.AddBlueprintAction(Action, Spawner);
	}
}

FText UK2NodeBangoBase::GetMenuCategory() const
{
	return LOCTEXT("RunScript_MenuCategory", "Bango|Scripting");
}

bool UK2NodeBangoBase::IsLatentForMacros() const
{
	return bIsLatent;
}

FName UK2NodeBangoBase::GetCornerIcon() const
{
	return (bIsLatent && !bHideLatentIcon) ? FName("Graph.Latent.LatentIcon") : NAME_None;
}

FText UK2NodeBangoBase::GetToolTipHeading() const
{
	return (bIsLatent) ? LOCTEXT("LatentFunc", "Latent") : FText::GetEmpty();
}

bool UK2NodeBangoBase::ShouldShowNodeProperties() const
{
	return bShowShowNodeProperties;
}

FSlateIcon UK2NodeBangoBase::GetIconAndTint(FLinearColor& OutColor) const
{
	//return Super::GetIconAndTint(OutColor);
	
	static const FSlateIcon Icon = FSlateIcon("BangoEditorStyleSet", "Icon.Plunger");
	return Icon;
}

FLinearColor UK2NodeBangoBase::GetNodeTitleColor() const
{
	return FLinearColor::Black;
}

#undef LOCTEXT_NAMEPSACE
