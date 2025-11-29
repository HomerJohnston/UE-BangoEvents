#pragma once

#include "BangoNamedClassBase.generated.h"

UCLASS(Abstract)
class UBangoNamedClassBase : public UObject
{
	GENERATED_BODY()
	
#if WITH_EDITORONLY_DATA
protected:
	/** Display name in the Trigger actor's selectors */
	UPROPERTY(EditDefaultsOnly, Category = "Bango")
	FText DisplayName;
	
	/** Affects sorting in the Trigger actor's selectors */
	UPROPERTY(EditDefaultsOnly, Category = "Bango")
	uint8 SortOrder = 0;
	
public:
	FText GetDisplayName() const { return DisplayName; }
	
	uint8 GetSortOrder() const { return SortOrder; }
#endif
	
#if WITH_EDITOR
public:
	template<typename T>
	static TArray<TSubclassOf<T>> GetAllTypes();
#endif
};

#if WITH_EDITOR
template <typename T>
TArray<TSubclassOf<T>> UBangoNamedClassBase::GetAllTypes()
{
	TArray<TSubclassOf<T>> Classes;
	
	// TODO load blueprint classes
	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;
		
		if (Class->HasAnyClassFlags(CLASS_Abstract))
		{
			continue;
		}
		
		if (Class->IsChildOf(T::StaticClass()))
		{
			Classes.Add(Class);
		}
	}
	
	// Now that we have all of them, we want to eliminate any types that have children. This lets games override the default provided classes.
	for (auto It = Classes.CreateIterator(); It; ++It)
	{
		auto CurrentTargetClass = *It;
		
		bool bHasChild = false;
		
		for (TSubclassOf<T> OtherClass : Classes)
		{
			if (OtherClass == CurrentTargetClass)
			{
				continue;
			}
			
			if (OtherClass->IsChildOf(CurrentTargetClass))
			{
				bHasChild = true;
				break;
			}
		}
		
		if (bHasChild)
		{
			// TODO -- only do this if the class doesn't have the override to stay in the list even with children (property on the target group class)
			It.RemoveCurrentSwap();
		}
	}
	
	Classes.Sort([] (const TSubclassOf<T>& X, const TSubclassOf<T>& Y)
	{
		///TSubclassOf<UBangoNamedClassBase> ASDF;
		
		///ASDF->GetDefaultObject<UBangoNamedClassBase>()->GetSortOrder();
		
		return X->GetDefaultObject<T>()->GetSortOrder() < Y->GetDefaultObject<T>()->GetSortOrder();
	});
	return Classes;
}
#endif