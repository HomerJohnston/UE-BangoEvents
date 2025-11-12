#include "BangoUncooked/K2/K2Node_BangoSleep.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_AddDelegate.h"
#include "K2Node_AssignmentStatement.h"
#include "K2Node_CallFunction.h"
#include "K2Node_CommutativeAssociativeBinaryOperator.h"
#include "K2Node_CreateDelegate.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_Self.h"
#include "K2Node_TemporaryVariable.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "KismetCompiler.h"
#include "Bango/Core/BangoScriptObject.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

class UK2Node_AssignmentStatement;

namespace K2Node_BangoSleepPins
{
	static const FName Duration(TEXT("Duration"));
	static const FName SkipExec(TEXT("SkipExec"));
	static const FName SkipCondition(TEXT("SkipCondition"));
	static const FName CancelExec(TEXT("CancelExec"));
	static const FName CancelCondition(TEXT("CancelCondition"));
}

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
	auto* DurationPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Real, K2Node_BangoSleepPins::Duration);
	DurationPin->DefaultValue = FString::SanitizeFloat(Duration);
	
	if (bEnableSkipExecPin)
	{
		UEdGraphPin* SkipExecPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, K2Node_BangoSleepPins::SkipExec);
		SkipExecPin->PinFriendlyName = LOCTEXT("SleepNodeSkipExecPinLabel", "Skip");
		SkipExecPin->PinToolTip = "If an input is received, the sleep will complete.";
	}
	
	if (bEnableCancelExecPin)
	{
		UEdGraphPin* CancelExecPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, K2Node_BangoSleepPins::CancelExec);
		CancelExecPin->PinFriendlyName = LOCTEXT("SleepNodeCancelExecPinLabel", "Cancel");
		CancelExecPin->PinToolTip = "If an input is received, the sleep will cancel. Output will NOT execute.";
	}
	
	if (bEnableSkipConditionPin)
	{
		UEdGraphPin* SkipConditionPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, K2Node_BangoSleepPins::SkipCondition);
		SkipConditionPin->PinFriendlyName = LOCTEXT("SleepNodeSkipConditionPinLabel", "Skip");
		SkipConditionPin->PinToolTip = "Polled on tick. If true, the sleep will complete.";
	}
	
	if (bEnableCancelConditionPin)
	{
		UEdGraphPin* CancelConditionPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, K2Node_BangoSleepPins::CancelCondition);
		CancelConditionPin->PinFriendlyName = LOCTEXT("SleepNodeCancelConditionPinLabel", "Cancel");
		CancelConditionPin->PinToolTip = "Polled on tick. If true, the sleep will cancel. Output will NOT execute.";
	}
}

void UK2Node_BangoSleep::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	check(SourceGraph && Schema);
	bool bIsErrorFree = true;
	UBlueprint* Blueprint = GetBlueprint();

	// Get input pins prepared for use
	UK2Node_BangoSleep* Node_This = this;
	UEdGraphPin* Node_This_Exec = Node_This->GetExecPin();
	UEdGraphPin* Node_This_Duration = Node_This->FindPin(K2Node_BangoSleepPins::Duration);
	UEdGraphPin* Node_This_SkipExec = Node_This->FindPin(K2Node_BangoSleepPins::SkipExec);
	UEdGraphPin* Node_This_CancelExec = Node_This->FindPin(K2Node_BangoSleepPins::CancelExec);
	UEdGraphPin* Node_This_SkipCondition = Node_This->FindPin(K2Node_BangoSleepPins::SkipCondition);
	UEdGraphPin* Node_This_CancelCondition = Node_This->FindPin(K2Node_BangoSleepPins::CancelCondition);
	UEdGraphPin* Node_This_Completed = Node_This->FindPin(UEdGraphSchema_K2::PN_Completed);
	
	// Spawn all nodes

	FVector2f GraphAnchor(5000, 0);

	// Internal properties (flags for triggering Skip / Cancel)
	MAKE_NODE(SkipExecTriggered, UK2Node_TemporaryVariable, 0, 1000);
	Node_SkipExecTriggered->VariableType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
	Node_SkipExecTriggered->AllocateDefaultPins();
	UEdGraphPin* Node_SkipExecTriggered_Variable = Node_SkipExecTriggered->GetVariablePin();

	MAKE_NODE(SetSkipExecTriggered, UK2Node_AssignmentStatement, 250, 1000);
	Node_SetSkipExecTriggered->AllocateDefaultPins();
	UEdGraphPin* Node_SetSkipExecTriggered_Exec = Node_SetSkipExecTriggered->GetExecPin();
	UEdGraphPin* Node_SetSkipExecTriggered_Then = Node_SetSkipExecTriggered->GetThenPin();
	UEdGraphPin* Node_SetSkipExecTriggered_Variable = Node_SetSkipExecTriggered->GetVariablePin();
	UEdGraphPin* Node_SetSkipExecTriggered_Value = Node_SetSkipExecTriggered->GetValuePin();
	
	MAKE_NODE(CancelExecTriggered, UK2Node_TemporaryVariable, 0, 1250);
	Node_CancelExecTriggered->VariableType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
	Node_CancelExecTriggered->AllocateDefaultPins();
	UEdGraphPin* Node_CancelExecTriggered_Variable = Node_CancelExecTriggered->GetVariablePin();

	MAKE_NODE(SetCancelExecTriggered, UK2Node_AssignmentStatement, 250, 1250);
	Node_SetCancelExecTriggered->AllocateDefaultPins();
	UEdGraphPin* Node_SetCancelExecTriggered_Exec = Node_SetCancelExecTriggered->GetExecPin();
	UEdGraphPin* Node_SetCancelExecTriggered_Then = Node_SetCancelExecTriggered->GetThenPin();
	UEdGraphPin* Node_SetCancelExecTriggered_Variable = Node_SetCancelExecTriggered->GetVariablePin();
	UEdGraphPin* Node_SetCancelExecTriggered_Value = Node_SetCancelExecTriggered->GetValuePin();

	/*
	MAKE_NODE(FinishTime, UK2Node_TemporaryVariable, 0, 250);
	Node_FinishTime->VariableType.PinCategory = UEdGraphSchema_K2::PC_Real;
	Node_FinishTime->VariableType.PinSubCategory = UEdGraphSchema_K2::PC_Double;
	Node_FinishTime->AllocateDefaultPins();
	UEdGraphPin* Node_FinishTime_Variable = Node_FinishTime->GetVariablePin();
	
	MAKE_NODE(SetFinishTime, UK2Node_AssignmentStatement, 250, 250);
	Node_SetFinishTime->AllocateDefaultPins();
	UEdGraphPin* Node_SetFinishTime_Exec = Node_SetFinishTime->GetExecPin();
	UEdGraphPin* Node_SetFinishTime_Then = Node_SetFinishTime->GetThenPin();
	UEdGraphPin* Node_SetFinishTime_Variable = Node_SetFinishTime->GetVariablePin();
	UEdGraphPin* Node_SetFinishTime_Value = Node_SetFinishTime->GetValuePin();
	*/
	
	MAKE_NODE_FUNC(LaunchSleep, UBangoScriptObject, LaunchSleep_Internal, 1000, 500);
	Node_LaunchSleep->AllocateDefaultPins();
	UEdGraphPin* Node_LaunchSleep_Exec = Node_LaunchSleep->GetExecPin();
	UEdGraphPin* Node_LaunchSleep_Then = Node_LaunchSleep->GetThenPin();
	UEdGraphPin* Node_LaunchSleep_Duration = Node_LaunchSleep->FindPin(TEXT("Duration"));
	UEdGraphPin* Node_LaunchSleep_TickDelegate = Node_LaunchSleep->FindPin(TEXT("TickDelegate"));
	UEdGraphPin* Node_LaunchSleep_CompleteDelegate = Node_LaunchSleep->FindPin(TEXT("CompleteDelegate"));
	UEdGraphPin* Node_LaunchSleep_UUID = Node_LaunchSleep->GetReturnValuePin();
	
	MAKE_NODE(ActionUUID, UK2Node_TemporaryVariable, 0, 250);
	Node_ActionUUID->VariableType.PinCategory = UEdGraphSchema_K2::PC_Int;
	Node_ActionUUID->AllocateDefaultPins();
	UEdGraphPin* Node_ActionUUID_Variable = Node_ActionUUID->GetVariablePin();
	
	MAKE_NODE(SetUUID, UK2Node_AssignmentStatement, 1250, 500);
	Node_SetUUID->AllocateDefaultPins();
	UEdGraphPin* Node_SetUUID_Exec = Node_SetUUID->GetExecPin();
	UEdGraphPin* Node_SetUUID_Then = Node_SetUUID->GetThenPin();
	UEdGraphPin* Node_SetUUID_Variable = Node_SetUUID->GetVariablePin();
	UEdGraphPin* Node_SetUUID_Value = Node_SetUUID->GetValuePin();

	/*
	MAKE_NODE_FUNC(GetGameTime, UKismetSystemLibrary, GetGameTimeInSeconds, -250, 750);
	Node_GetGameTime->AllocateDefaultPins();
	UEdGraphPin* Node_GetGameTime_ReturnValue = Node_GetGameTime->FindPin(UEdGraphSchema_K2::PN_ReturnValue);
	*/
	
	/*
	MAKE_NODE_FUNC(Add, UKismetMathLibrary, Add_DoubleDouble, 0, 500);
	Node_Add->AllocateDefaultPins();
	UEdGraphPin* Node_Add_A = Node_Add->FindPin(TEXT("A"));
	UEdGraphPin* Node_Add_B = Node_Add->FindPin(TEXT("B"));
	UEdGraphPin* Node_Add_ReturnValue = Node_Add->GetReturnValuePin();
	*/

	// FBlueprintEditorUtils::FindUniqueCustomEventName does not work
	FGuid UniqueID = FGuid::NewGuid();
	
	MAKE_NODE(TickEvent, UK2Node_CustomEvent, 500, 2000);
	Node_TickEvent->CustomFunctionName = FName(UniqueID.ToString(), 0);
	Node_TickEvent->AllocateDefaultPins();
	UEdGraphPin* Node_TickEvent_Then = Node_TickEvent->GetThenPin();
	UEdGraphPin* Node_TickEvent_Delegate = Node_TickEvent->GetDelegatePin();
	
	MAKE_NODE(CompleteEvent, UK2Node_CustomEvent, 2500, 1000);
	Node_CompleteEvent->CustomFunctionName = FName(UniqueID.ToString(), 1);
	Node_CompleteEvent->AllocateDefaultPins();
	UEdGraphPin* Node_CompleteEvent_Then = Node_CompleteEvent->GetThenPin();
	UEdGraphPin* Node_CompleteEvent_Delegate = Node_CompleteEvent->GetDelegatePin();

	MAKE_NODE(CancelBranch, UK2Node_IfThenElse, 1000, 500);
	Node_CancelBranch->AllocateDefaultPins();
	UEdGraphPin* Node_CancelBranch_Exec = Node_CancelBranch->GetExecPin();
	UEdGraphPin* Node_CancelBranch_Condition = Node_CancelBranch->GetConditionPin();
	UEdGraphPin* Node_CancelBranch_Then = Node_CancelBranch->GetThenPin();
	UEdGraphPin* Node_CancelBranch_Else = Node_CancelBranch->GetElsePin();

	MAKE_NODE(SkipBranch, UK2Node_IfThenElse, 1000, 1000);
	Node_SkipBranch->AllocateDefaultPins();
	UEdGraphPin* Node_SkipBranch_Exec = Node_SkipBranch->GetExecPin();
	UEdGraphPin* Node_SkipBranch_Condition = Node_SkipBranch->GetConditionPin();
	UEdGraphPin* Node_SkipBranch_Then = Node_SkipBranch->GetThenPin();
	UEdGraphPin* Node_SkipBranch_Else = Node_SkipBranch->GetElsePin();

	MAKE_NODE_FUNC(CancelConditionsOR, UKismetMathLibrary, BooleanOR, 750, 500);
	Node_CancelConditionsOR->AllocateDefaultPins();
	UEdGraphPin* Node_CancelConditionsOR_A = Node_CancelConditionsOR->FindPin(TEXT("A"));
	UEdGraphPin* Node_CancelConditionsOR_B = Node_CancelConditionsOR->FindPin(TEXT("B"));
	UEdGraphPin* Node_CancelConditionsOR_Value = Node_CancelConditionsOR->GetReturnValuePin();
	
	MAKE_NODE_FUNC(SkipConditionsOR, UKismetMathLibrary, BooleanOR, 750, 1000);
	Node_SkipConditionsOR->AllocateDefaultPins();
	UEdGraphPin* Node_SkipConditionsOR_A = Node_SkipConditionsOR->FindPin(TEXT("A"));
	UEdGraphPin* Node_SkipConditionsOR_B = Node_SkipConditionsOR->FindPin(TEXT("B"));
	UEdGraphPin* Node_SkipConditionsOR_Value = Node_SkipConditionsOR->GetReturnValuePin();

	MAKE_NODE_FUNC(CancelSleep, UBangoScriptObject, CancelSleep_Internal, 1500, 500);
	Node_CancelSleep->AllocateDefaultPins();
	UEdGraphPin* Node_CancelSleep_Exec = Node_CancelSleep->GetExecPin();
	UEdGraphPin* Node_CancelSleep_Then = Node_CancelSleep->GetThenPin();
	UEdGraphPin* Node_CancelSleep_ActionUUID = Node_CancelSleep->FindPin(TEXT("ActionUUID"));
	
	MAKE_NODE_FUNC(SkipSleep, UBangoScriptObject, SkipSleep_Internal, 1500, 1000);
	Node_SkipSleep->AllocateDefaultPins();
	UEdGraphPin* Node_SkipSleep_Exec = Node_SkipSleep->GetExecPin();
	UEdGraphPin* Node_SkipSleep_Then = Node_SkipSleep->GetThenPin();
	UEdGraphPin* Node_SkipSleep_ActionUUID = Node_SkipSleep->FindPin(TEXT("ActionUUID"));
	
	// Set up default pin values
	Node_SetCancelExecTriggered_Value->DefaultValue = "True";
	Node_SetSkipExecTriggered_Value->DefaultValue = "True";
	
	// Wire up nodes

	// Launch Sleep inputs
	bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*Node_This_Exec, *Node_LaunchSleep_Exec).CanSafeConnect();
	bIsErrorFree &= Schema->TryCreateConnection(Node_TickEvent_Delegate, Node_LaunchSleep_TickDelegate);
	bIsErrorFree &= Schema->TryCreateConnection(Node_CompleteEvent_Delegate, Node_LaunchSleep_CompleteDelegate);

	if (Node_This_Duration->HasAnyConnections())
	{
		bIsErrorFree &= CompilerContext.CopyPinLinksToIntermediate(*Node_This_Duration, *Node_LaunchSleep_Duration).CanSafeConnect();
	}
	else
	{
		Node_LaunchSleep_Duration->DefaultValue = Node_This_Duration->DefaultValue;
	}
	
	// Set UUID inputs
	bIsErrorFree &= Schema->TryCreateConnection(Node_LaunchSleep_Then, Node_SetUUID_Exec);
	bIsErrorFree &= Schema->TryCreateConnection(Node_ActionUUID_Variable, Node_SetUUID_Variable);
	bIsErrorFree &= Schema->TryCreateConnection(Node_LaunchSleep_UUID, Node_SetUUID_Value);

	// OR Cancel conditions inputs
	if (Node_This_CancelCondition)
	{
		bIsErrorFree &= CompilerContext.CopyPinLinksToIntermediate(*Node_This_CancelCondition, *Node_CancelConditionsOR_B).CanSafeConnect();	
	}

	// OR Skip conditions inputs
	if (Node_This_SkipCondition)
	{
		bIsErrorFree &= CompilerContext.CopyPinLinksToIntermediate(*Node_This_SkipCondition, *Node_SkipConditionsOR_B).CanSafeConnect();
	}

	// Branch Cancel inputs
	bIsErrorFree &= Schema->TryCreateConnection(Node_TickEvent_Then, Node_CancelBranch_Exec);
	bIsErrorFree &= Schema->TryCreateConnection(Node_CancelConditionsOR_Value, Node_CancelBranch_Condition);

	// Branch Skip inputs
	bIsErrorFree &= Schema->TryCreateConnection(Node_CancelBranch_Else, Node_SkipBranch_Exec);
	bIsErrorFree &= Schema->TryCreateConnection(Node_SkipConditionsOR_Value, Node_SkipBranch_Condition);

	// Cancel Sleep function inputs
	bIsErrorFree &= Schema->TryCreateConnection(Node_CancelBranch_Then, Node_CancelSleep_Exec);
	bIsErrorFree &= Schema->TryCreateConnection(Node_ActionUUID_Variable, Node_CancelSleep_ActionUUID);

	// Skip Sleep function inputs
	bIsErrorFree &= Schema->TryCreateConnection(Node_SkipBranch_Then, Node_SkipSleep_Exec);
	bIsErrorFree &= Schema->TryCreateConnection(Node_ActionUUID_Variable, Node_SkipSleep_ActionUUID);
		
	// Final output
	bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*Node_This_Completed, *Node_CompleteEvent_Then).CanSafeConnect();
	
	// Exec Skip/Cancel inputs
	if (Node_This_CancelExec)
	{
		bIsErrorFree &= Schema->TryCreateConnection(Node_CancelExecTriggered_Variable, Node_CancelConditionsOR_A);
		bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*Node_This_CancelExec, *Node_SetCancelExecTriggered_Exec).CanSafeConnect();
		bIsErrorFree &= Schema->TryCreateConnection(Node_CancelExecTriggered_Variable, Node_SetCancelExecTriggered_Variable);
	}
	if (Node_This_SkipExec)
	{
		bIsErrorFree &= Schema->TryCreateConnection(Node_SkipExecTriggered_Variable, Node_SkipConditionsOR_A);
		bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*Node_This_SkipExec, *Node_SetSkipExecTriggered_Exec).CanSafeConnect();
		bIsErrorFree &= Schema->TryCreateConnection(Node_SkipExecTriggered_Variable, Node_SetSkipExecTriggered_Variable);
	}


	// Done!
	if (!bIsErrorFree)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("InternalConnectionError", "K2Node_LoadAsset: Internal connection error. @@").ToString(), this);
	}
	
	// Disconnect ThisNode from the graph
	BreakAllNodeLinks();
}

#undef LOCTEXT_NAMESPACE
