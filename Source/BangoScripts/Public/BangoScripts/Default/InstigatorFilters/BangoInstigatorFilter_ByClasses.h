#pragma once
#include "BangoScripts/BangoInstigatorFilter.h"

#include "BangoInstigatorFilter_ByClasses.generated.h"

UCLASS(DisplayName="By Classes")
class BANGOSCRIPTS_API UBangoInstigatorFilter_ByClasses : public UBangoInstigatorFilter
{
	GENERATED_BODY()
	
	// ============================================================================================
	// SETTINGS
	// ============================================================================================
protected:
	
	/** Only instigators of these types will be considered. (If behavior is inverted, this will instead only be used to override the block list.) */
	UPROPERTY(EditAnywhere, Category="Settings", DisplayName="Allow List")
	TArray<TSubclassOf<UObject>> AllowList;

	/** If true, then only exact classes are compared. By default, object inheritance is respected. */
	UPROPERTY(EditAnywhere, Category="Settings")
	bool bIgnoreChildClassesForAllowList = false;

	/** Use this to prohibit specific children from the allow list. (If behavior is inverted, this will instead be used to determine which instigators to ignore.) */
	UPROPERTY(EditAnywhere, Category="Settings", DisplayName="Block List")
	TArray<TSubclassOf<UObject>> IgnoreList;
	
	/** If true, then only exact classes are compared. By default, object inheritance is respected. */
	UPROPERTY(EditAnywhere, Category="Settings")
	bool bIgnoreChildClassesForIgnoreList = false;

	/** If true, behaviour is reversed; all classes will be allowed, then the block list will be used instead to prevent specific classes from being allowed, and the allow list will override the block list for any specified children. */
	UPROPERTY(EditAnywhere, Category="Settings")	
	bool bInvertBehavior = false;
	
public:
	bool IsValidInstigator_Implementation(UObject* Instigator) override;
};