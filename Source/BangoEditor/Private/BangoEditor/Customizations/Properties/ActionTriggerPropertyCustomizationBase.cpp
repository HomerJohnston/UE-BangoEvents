#include "BangoEditor/Customizations/Properties/ActionTriggerPropertyCustomizationBase.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "Bango/BangoAction.h"
#include "Bango/Utility/BangoLog.h"
#include "PropertyEditor/Private/CategoryPropertyNode.h"
#include "PropertyEditor/Private/PropertyNode.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/SPanel.h"

FGroupProperties::FGroupProperties() : GroupName(NAME_None)
{
}

FGroupProperties::FGroupProperties(FName InGroupName) : GroupName(InGroupName)
{
}

FBangoActionTriggerPropertyCustomizationBase::FBangoActionTriggerPropertyCustomizationBase()
{
	HeaderColor = FLinearColor::Black;
}

TSharedRef<IPropertyTypeCustomization> FBangoActionTriggerPropertyCustomizationBase::MakeInstance()
{
	return MakeShareable(new FBangoActionTriggerPropertyCustomizationBase());
}

void FBangoActionTriggerPropertyCustomizationBase::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedRef<SWidget> PropertyNameWidget = PropertyHandle->CreatePropertyNameWidget();
	TSharedRef<SWidget> PropertyValueWidget = PropertyHandle->CreatePropertyValueWidget();
	
	HeaderRow.WholeRowContent()
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SColorBlock)
			.Color(HeaderColor)
		]
		+ SOverlay::Slot()
		[
			PropertyValueWidget
		]
	];
}


void FBangoActionTriggerPropertyCustomizationBase::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{	
	UObject* InstancedActionObject = nullptr;
	PropertyHandle->GetValue(InstancedActionObject);

	if (!IsValid(InstancedActionObject))
	{
		return;
	}

	uint32 NumClasses;
	PropertyHandle->GetNumChildren(NumClasses);

	for (uint32 i = 0; i < NumClasses; i++)
	{
		TSharedRef<IPropertyHandle> ClassRef = PropertyHandle->GetChildHandle(i).ToSharedRef();

		uint32 NumCategories;
		ClassRef->GetNumChildren(NumCategories);

		TArray<FGroupProperties> Groups;

		FGroupProperties ActionGroup;
		
		FGroupProperties DefaultGroup("Default");

		FGroupProperties AdvancedGroup("Advanced");
		
		for (uint32 j = 0; j < NumCategories; j++)
		{
			TSharedRef<IPropertyHandle> CategoryRef = ClassRef->GetChildHandle(j).ToSharedRef();

			FCategoryPropertyNode* CategoryNode = CategoryRef->GetPropertyNode()->AsCategoryNode();

			if (!CategoryNode)
			{
				continue;
			}

			FName CategoryName = CategoryNode->GetCategoryName();

			FGroupProperties GroupProperties;
			GroupProperties.GroupName = CategoryName;

			uint32 NumProperties;
			CategoryRef->GetNumChildren(NumProperties);

			for (uint32 k = 0; k < NumProperties; k++)
			{
				TSharedRef<IPropertyHandle> PropertyRef = CategoryRef->GetChildHandle(k).ToSharedRef();

				if (PropertyRef->GetProperty()->HasAnyPropertyFlags(CPF_Transient))
				{
					continue;
				}

				TSharedPtr<FBangoPropertyDrawer>* PropertyDrawerPtr = PropertyDrawers.Find(PropertyRef->GetProperty()->GetFName());

				if (PropertyDrawerPtr)
				{
					(*PropertyDrawerPtr)->Draw(PropertyRef, ChildBuilder);
					continue;
				}
				
				if (CategoryName == InstancedActionObject->GetClass()->GetFName())
				{
					ActionGroup.Properties.Add(PropertyRef);
				}
				else if (CategoryName == "Default")
				{
					DefaultGroup.Properties.Add(PropertyRef);
				}
				else if (CategoryName == "Advanced")
				{
					AdvancedGroup.Properties.Add(PropertyRef);
				}
				else
				{
					GroupProperties.Properties.Add(PropertyRef);
				}
			}

			Groups.Add(GroupProperties);
		}

		DrawGroup(DefaultGroup, ChildBuilder, false);

		DrawGroup(ActionGroup, ChildBuilder, false);
		
		for (FGroupProperties& GroupProperties : Groups)
		{
			DrawGroup(GroupProperties, ChildBuilder, true);
		}

		DrawGroup(AdvancedGroup, ChildBuilder, true);
	}
}

void FBangoActionTriggerPropertyCustomizationBase::DrawGroup(FGroupProperties& Group, IDetailChildrenBuilder& ChildBuilder, bool bUseGroup)
{
	if (Group.Properties.IsEmpty())
	{
		return;
	}

	if (bUseGroup)
	{
		IDetailGroup& DetailGroup = ChildBuilder.AddGroup(Group.GroupName, FText::FromName(Group.GroupName));

		for (TSharedRef<IPropertyHandle>& PropertyHandleRef : Group.Properties)
		{
			DetailGroup.AddPropertyRow(PropertyHandleRef);
		}
	}
	else
	{
		for (TSharedRef<IPropertyHandle>& PropertyHandleRef : Group.Properties)
		{
			ChildBuilder.AddProperty(PropertyHandleRef);
		}
	}
}

