#include "BangoEditor/Customizations/Properties/ActionPropertyCustomization.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "Bango/BangoAction.h"
#include "Bango/Utility/BangoLog.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/SPanel.h"

void FBangoActionPropertyCustomization_ActionSelectorDrawer::Draw(TSharedRef<IPropertyHandle> PropertyRef, IDetailChildrenBuilder& ChildBuilder)
{
	TSharedPtr<IPropertyHandle> ParentHandle = PropertyRef->GetParentHandle()->GetParentHandle()->GetParentHandle();
	UObject* InstancedActionObject = nullptr;
	ParentHandle->GetValue(InstancedActionObject);

	UBangoAction* InstancedAction = Cast<UBangoAction>(InstancedActionObject);

	if (!IsValid(InstancedAction))
	{
		return;
	}

	GenerateComboboxEntries(ParentHandle);
	
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

void FBangoActionPropertyCustomization_ActionSelectorDrawer::GenerateComboboxEntries(TSharedPtr<IPropertyHandle> PropertyHandle)
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
		}
		else
		{
			UE_LOG(Bango, Error, TEXT("Invalid action name registered in <%s> - could not find UFUNCTION() named <%s>! Ignoring."), *InstancedAction->GetClass()->GetFName().ToString(), *FunctionName.ToString());
		}
	}
}

FBangoActionPropertyCustomization::FBangoActionPropertyCustomization()
{
	HeaderColor = FLinearColor(0.15, 0.05, 0.04);

	PropertyDrawers.Add( GET_MEMBER_NAME_CHECKED(UBangoAction, OnEventActivate), MakeShareable(new FBangoActionPropertyCustomization_ActionSelectorDrawer()));
	PropertyDrawers.Add( GET_MEMBER_NAME_CHECKED(UBangoAction, OnEventDeactivate), MakeShareable(new FBangoActionPropertyCustomization_ActionSelectorDrawer()));
}

TSharedRef<IPropertyTypeCustomization> FBangoActionPropertyCustomization::MakeInstance()
{
	return MakeShareable(new FBangoActionPropertyCustomization());
}
