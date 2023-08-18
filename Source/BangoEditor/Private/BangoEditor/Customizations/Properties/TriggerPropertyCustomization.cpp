#include "BangoEditor/Customizations/Properties/TriggerPropertyCustomization.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "Widgets/Colors/SColorBlock.h"

FBangoTriggerPropertyCustomization::FBangoTriggerPropertyCustomization()
{
}

TSharedRef<IPropertyTypeCustomization> FBangoTriggerPropertyCustomization::MakeInstance()
{
	return MakeShareable(new FBangoTriggerPropertyCustomization());
}

void FBangoTriggerPropertyCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedRef<SWidget> PropertyValueWidget = PropertyHandle->CreatePropertyValueWidget();
	
	HeaderRow.WholeRowContent()
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SColorBlock)
			.Color(FLinearColor(0.05, 0.04, 0.15))
		]
		+ SOverlay::Slot()
		[
			PropertyValueWidget
		]
	];
}

void FBangoTriggerPropertyCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	uint32 NumClasses;
	PropertyHandle->GetNumChildren(NumClasses);

	static TArray<FName> EventTriggerProperties = {"WhenEventActivates", "WhenEventDeactivates"};
	
	for (uint32 i = 0; i < NumClasses; i++)
	{
		TSharedRef<IPropertyHandle> ClassRef = PropertyHandle->GetChildHandle(i).ToSharedRef();

		uint32 NumCategories;
		ClassRef->GetNumChildren(NumCategories);

		// Iterate backward through categories because the engine always lists inherited children categories first, and I want them to be listed last (show base class properties first)
		for (int32 j = NumCategories - 1; j >= 0; j--)
		{
			TSharedRef<IPropertyHandle> CategoryRef = ClassRef->GetChildHandle(j).ToSharedRef();

			uint32 NumProperties;
			CategoryRef->GetNumChildren(NumProperties);

			for (uint32 k = 0; k < NumProperties; k++)
			{
				TSharedRef<IPropertyHandle> PropertyRef = CategoryRef->GetChildHandle(k).ToSharedRef();

				if (PropertyRef->GetProperty()->HasAnyPropertyFlags(CPF_Transient))
					continue;
				
				ChildBuilder.AddProperty(PropertyRef);
			}
		}
	}
}
