#include "BangoUncooked/K2Nodes/Base/_K2NodeBangoBase.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

UK2Node_BangoBase::UK2Node_BangoBase()
{
	MenuSubcategory = FText::GetEmpty();
}

void UK2Node_BangoBase::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* Action = GetClass();
	
	if (ActionRegistrar.IsOpenForRegistration(Action))
	{
		UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(Action);
		check(Spawner);

		ActionRegistrar.AddBlueprintAction(Action, Spawner);
	}
}

FText UK2Node_BangoBase::GetMenuCategory() const
{
	return FText::Join(INVTEXT("|"), INVTEXT("Bango"), MenuSubcategory);
}

bool UK2Node_BangoBase::IsLatentForMacros() const
{
	return bIsLatent;
}

FName UK2Node_BangoBase::GetCornerIcon() const
{
	return (bIsLatent && !bHideLatentIcon) ? FName("Graph.Latent.LatentIcon") : NAME_None;
}

FText UK2Node_BangoBase::GetToolTipHeading() const
{
	return (bIsLatent) ? LOCTEXT("LatentFunc", "Latent") : FText::GetEmpty();
}

bool UK2Node_BangoBase::ShouldShowNodeProperties() const
{
	return bShowNodeProperties;
}

FSlateIcon UK2Node_BangoBase::GetIconAndTint(FLinearColor& OutColor) const
{
	//return Super::GetIconAndTint(OutColor);
	
	return FSlateIcon();
	
	//static const FSlateIcon Icon = FSlateIcon("BangoEditorStyleSet", "Icon.Plunger_Dim");
	//return Icon;
}

FLinearColor UK2Node_BangoBase::GetNodeTitleColor() const
{
	return FLinearColor::Black;
}

FLinearColor UK2Node_BangoBase::GetNodeTitleTextColor() const
{
	return FLinearColor::Gray;
}

#undef LOCTEXT_NAMEPSACE
