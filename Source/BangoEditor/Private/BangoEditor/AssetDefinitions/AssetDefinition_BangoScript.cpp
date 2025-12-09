#include "BangoEditor/AssetDefinitions/AssetDefinition_BangoScript.h"

#include "Bango/Core/BangoScript.h"

TConstArrayView<FAssetCategoryPath> UAssetDefinition_BangoScript::GetAssetCategories() const
{
	return Super::GetAssetCategories();
}

TSoftClassPtr<UObject> UAssetDefinition_BangoScript::GetAssetClass() const
{
	return UBangoScript::StaticClass();
}

FLinearColor UAssetDefinition_BangoScript::GetAssetColor() const
{
	// TODO
	return FLinearColor::Black;
}

FText UAssetDefinition_BangoScript::GetAssetDescription(const FAssetData& AssetData) const
{
	return INVTEXT("TODO test Bango Script Description");
}

FText UAssetDefinition_BangoScript::GetAssetDisplayName(const FAssetData& AssetData) const
{
	return INVTEXT("TODO test Bango Script");
}
