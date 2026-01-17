#pragma once
#include "AssetDefinitionDefault.h"

#include "AssetDefinition_BangoScript.generated.h"

UCLASS()
class UAssetDefinition_BangoScript : public UAssetDefinitionDefault
{
	GENERATED_BODY()

public:
	TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	TSoftClassPtr<UObject> GetAssetClass() const override;
	FLinearColor GetAssetColor() const override;
	FText GetAssetDescription(const FAssetData& AssetData) const override;
	FText GetAssetDisplayName(const FAssetData& AssetData) const override;
};