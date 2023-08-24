#include "BangoEditor/Customizations/Properties/ActionPropertyCustomization.h"

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

FBangoActionPropertyCustomization::FBangoActionPropertyCustomization()
{
}

TSharedRef<IPropertyTypeCustomization> FBangoActionPropertyCustomization::MakeInstance()
{
	return MakeShareable(new FBangoActionPropertyCustomization());
}

void FBangoActionPropertyCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedRef<SWidget> PropertyNameWidget = PropertyHandle->CreatePropertyNameWidget();
	TSharedRef<SWidget> PropertyValueWidget = PropertyHandle->CreatePropertyValueWidget();

	/*
	HeaderRow.NameContent()
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SColorBlock)
			.Color(FLinearColor(0.15, 0.05, 0.04))
		]
		+ SOverlay::Slot()
		[
			PropertyNameWidget
		]
	];

	HeaderRow.ValueContent()
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SColorBlock)
			.Color(FLinearColor(0.15, 0.05, 0.04))
		]
		+ SOverlay::Slot()
		[
			PropertyValueWidget
		]
	];
	*/
	
	HeaderRow.WholeRowContent()
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SColorBlock)
			.Color(FLinearColor(0.15, 0.05, 0.04))
		]
		+ SOverlay::Slot()
		[
			PropertyValueWidget
		]
	];
}

struct FGroupProperties
{
	FName GroupName;
	TArray<TSharedRef<IPropertyHandle>> Properties;
};

void FBangoActionPropertyCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	uint32 NumClasses;
	PropertyHandle->GetNumChildren(NumClasses);

	static TArray<FName> EventTriggerProperties = { GET_MEMBER_NAME_CHECKED(UBangoAction, OnEventActivate), GET_MEMBER_NAME_CHECKED(UBangoAction, OnEventDeactivate) };

	GenerateComboboxEntries(PropertyHandle);
	
	for (uint32 i = 0; i < NumClasses; i++)
	{
		TSharedRef<IPropertyHandle> ClassRef = PropertyHandle->GetChildHandle(i).ToSharedRef();

		uint32 NumCategories;
		ClassRef->GetNumChildren(NumCategories);

		TArray<FGroupProperties> Groups;
		
		// Iterate backward through categories because the engine always lists inherited children categories first, and I want them to be listed last (show base class properties first)
		//for (uint32 j = NumCategories - 1; j >= 0; j--)
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
				
				if (EventTriggerProperties.Contains(PropertyRef->GetProperty()->GetFName()))
				{
					DrawActionSelector(PropertyRef, PropertyHandle, ChildBuilder, CustomizationUtils);
					continue;
				}

				if (CategoryName == "Settings")
				{
					ChildBuilder.AddProperty(PropertyRef);
				}
				else
				{
					GroupProperties.Properties.Add(PropertyRef);
				}
			}

			Groups.Add(GroupProperties);
		}

		for (FGroupProperties& GroupProperties : Groups)
		{
			if (GroupProperties.Properties.IsEmpty())
			{
				continue;
			}
			
			IDetailGroup& Group = ChildBuilder.AddGroup(GroupProperties.GroupName, FText::FromName(GroupProperties.GroupName));

			for (TSharedRef<IPropertyHandle>& PropertyHandleRef : GroupProperties.Properties)
			{
				Group.AddPropertyRow(PropertyHandleRef);
			}
		}
	}
}

void FBangoActionPropertyCustomization::DrawActionSelector(TSharedRef<IPropertyHandle> PropertyRef, TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	UObject* InstancedActionObject = nullptr;
	PropertyHandle->GetValue(InstancedActionObject);
	UBangoAction* InstancedAction = Cast<UBangoAction>(InstancedActionObject);

	if (!IsValid(InstancedAction))
	{
		return;
	}
	
	FName CurrentFunctionName;
	FString CurrentAction;

	PropertyRef->GetValue(CurrentFunctionName);

	UFunction* Function = InstancedAction->FindFunction(CurrentFunctionName);

	if (CurrentFunctionName == NAME_None)
	{
		CurrentAction = "Do Nothing";
	}
	else if (Function && ActionFunctionNames.Contains(CurrentFunctionName))
	{
		CurrentAction = FName::NameToDisplayString(CurrentFunctionName.ToString(), false);
	}
	else
	{
		CurrentAction = FString::Printf(TEXT("ERROR: <%s> NOT FOUND"), *CurrentFunctionName.ToString());
	}
	
	auto Font = FAppStyle::Get().GetFontStyle("SmallFont");
	auto Color = FLinearColor::Red;
	
	TSharedPtr<STextBlock>& ComboBoxTitleBlock = ComboBoxTitleBlocks.FindOrAdd(PropertyRef->GetProperty()->GetFName());
	
	auto ComboBox = SNew(SComboBox<TSharedPtr<FString> >)
	.OptionsSource(&ActionFunctionDescriptions)
	.OnGenerateWidget_Lambda([Font](TSharedPtr<FString> Item) 
	{ 
		return SNew(STextBlock)
		.Text(FText::FromString(*Item))
		.Font(Font);
	} )
	.OnSelectionChanged_Lambda([this, PropertyRef, InstancedAction] (TSharedPtr<FString> InSelection, ESelectInfo::Type InSelectInfo) 
	{
		TSharedPtr<STextBlock>* ComboBoxTitleBlock = ComboBoxTitleBlocks.Find(PropertyRef->GetProperty()->GetFName());
		
		if (!InSelection.IsValid() || !ComboBoxTitleBlock->IsValid())
		{
			return;
		}
		
		(*ComboBoxTitleBlock)->SetText( FText::FromString(*InSelection));

		int32 Index = ActionFunctionDescriptions.IndexOfByPredicate([&](TSharedPtr<FString> S) -> bool
		{
			return (*S).Equals(*InSelection);
		});

		if (Index != INDEX_NONE)
		{
			PropertyRef->SetValue(ActionFunctionNames[Index]);
		}
	})
	[
		SAssignNew(ComboBoxTitleBlock, STextBlock)
		.Text(FText::FromString(CurrentAction))
		.Font(Font)
		.HighlightColor(Color)
	];
	
	const FText ComboBoxRowName = INVTEXT("Test");
	if (!ComboBoxRowName.IsEmpty())
	{
		FDetailWidgetRow& CustomRow = ChildBuilder.AddCustomRow(ComboBoxRowName);

		CustomRow.NameContent()
		[
			PropertyRef->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			ComboBox
		];
	}
}

void FBangoActionPropertyCustomization::GenerateComboboxEntries(TSharedRef<IPropertyHandle> PropertyHandle)
{	
	UObject* InstancedActionObject = nullptr;
	PropertyHandle->GetValue(InstancedActionObject);
	UBangoAction* InstancedAction = Cast<UBangoAction>(InstancedActionObject);

	if (!IsValid(InstancedAction))
	{
		return;
	}
	
	ActionFunctionNames.Add(NAME_None);
	ActionFunctionDescriptions.Add(MakeShareable(new FString("Do Nothing")));
	ActionFunctionNameColors.Add(FLinearColor::White);

	for (FName FunctionName : InstancedAction->ActionFunctions)
	{
		UFunction* Function = InstancedAction->FindFunction(FunctionName);

		if (FunctionName == NAME_None)
		{
			UE_LOG(Bango, Error, TEXT("Invalid action name registered in <%s> - cannot use None as a function name! Ignoring."), *InstancedAction->GetClass()->GetFName().ToString());
		}
		else if (Function)
		{
			ActionFunctionNames.Add(FunctionName);
			ActionFunctionDescriptions.Add(MakeShareable(new FString(FName::NameToDisplayString(FunctionName.ToString(), false))));
			ActionFunctionNameColors.Add(FLinearColor::Green);
		}
		else
		{
			UE_LOG(Bango, Error, TEXT("Invalid action name registered in <%s> - could not find UFUNCTION() named <%s>! Ignoring."), *InstancedAction->GetClass()->GetFName().ToString(), *FunctionName.ToString());
		}
	}
}
