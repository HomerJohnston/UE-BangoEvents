#include "BangoUncooked/K2/K2Node_BangoSleep.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

UK2Node_BangoSleep::UK2Node_BangoSleep()
{
	bIsLatent = true;
	bHideLatentIcon = true;
	bShowShowNodeProperties = true;
}

void UK2Node_BangoSleep::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	ReconstructNode();

	Super::PostEditChangeProperty(PropertyChangedEvent);
    
	GetGraph()->NotifyNodeChanged(this);
}

void UK2Node_BangoSleep::AllocateDefaultPins()
{
	auto* ExecutePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	auto* CompletedPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Completed);
	auto* DurationPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Real, FName("Duration"));
	DurationPin->DefaultValue = FString::SanitizeFloat(Duration);
	
	if (bEnableSkipExecPin)
	{
		UEdGraphPin* SkipExecPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, FName("SkipExec"));
		SkipExecPin->PinFriendlyName = LOCTEXT("SleepNodeSkipExecPinLabel", "Skip");
		SkipExecPin->PinToolTip = "If an input is received, the sleep will complete.";
	}
	
	if (bEnableCancelExecPin)
	{
		UEdGraphPin* CancelExecPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, FName("CancelExec"));
		CancelExecPin->PinFriendlyName = LOCTEXT("SleepNodeCancelExecPinLabel", "Cancel");
		CancelExecPin->PinToolTip = "If an input is received, the sleep will cancel. Output will NOT execute.";
	}
	
	if (bEnableSkipConditionPin)
	{
		UEdGraphPin* SkipConditionPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, FName("SkipCondition"));
		SkipConditionPin->PinFriendlyName = LOCTEXT("SleepNodeSkipConditionPinLabel", "Skip");
		SkipConditionPin->PinToolTip = "Polled on tick. If true, the sleep will complete.";
	}
	
	if (bEnableCancelConditionPin)
	{
		UEdGraphPin* CancelConditionPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, FName("CancelCondition"));
		CancelConditionPin->PinFriendlyName = LOCTEXT("SleepNodeCancelConditionPinLabel", "Cancel");
		CancelConditionPin->PinToolTip = "Polled on tick. If true, the sleep will cancel. Output will NOT execute.";
	}

}

void UK2Node_BangoSleep::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
}

#undef LOCTEXT_NAMESPACE