#include "BangoScripts/Editor/Customizations/Properties/ActionTriggerPropertyCustomizationBase.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "BangoScripts/BangoAction.h"
#include "BangoScripts/Utility/BangoScriptsLog.h"
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

	// TODO WHAT IS THIS!??!?!?!
	// FPropertyEditorModule& PropertyEditor = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	// PropertyEditor.CreateFloatingDetailsView()


	
	
	
	
	UObject* InstancedActionObject = nullptr;
	PropertyHandle->GetValue(InstancedActionObject);

	if (!IsValid(InstancedActionObject))
	{
		return;
	}

	TMap<FName, FGroupProperties> PropertyGroups;

	uint32 NumClasses;
	PropertyHandle->GetNumChildren(NumClasses);

	for (uint32 i = 0; i < NumClasses; i++)
	{
		TSharedPtr<IPropertyHandle> ClassRef = PropertyHandle->GetChildHandle(i);

		uint32 NumCategories;
		ClassRef->GetNumChildren(NumCategories);
		
		for (uint32 j = 0; j < NumCategories; j++)
		{
			TSharedPtr<IPropertyHandle> CategoryPtr = ClassRef->GetChildHandle(j);

			uint32 NumProperties;

			CategoryPtr->GetNumChildren(NumProperties);
			
			for (uint32 k = 0; k < NumProperties; k++)
			{
				TSharedRef<IPropertyHandle> PropertyRef = CategoryPtr->GetChildHandle(k).ToSharedRef();

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
				
				FName CategoryName = FName(PropertyRef->GetMetaData("Category"));

				FGroupProperties& PropertyGroup = PropertyGroups.FindOrAdd(CategoryName, FGroupProperties());

				PropertyGroup.GroupName = CategoryName;
				
				PropertyGroup.Properties.Add(PropertyRef);
			}
		}
		
		for (auto& [CategoryName, PropertyGroup] : PropertyGroups)
		{
			DrawGroup(PropertyGroup, ChildBuilder, true);
		}
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

