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
#include "BangoUncooked/BangoNodeBuilder.h"
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

	BangoK2NodeBuilder::Setup(CompilerContext, SourceGraph, this);
	
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
	MAKE_NODE(TemporaryVariable, SkipExecTriggered, 0, 1000, Deferred);
	Node_SkipExecTriggered->VariableType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
	Node_SkipExecTriggered.FinishDeferredConstruction();

	MAKE_BASIC_NODE_OLD(SetSkipExecTriggered, UK2Node_AssignmentStatement, 250, 1000);
	UEdGraphPin* Node_SetSkipExecTriggered_Exec = Node_SetSkipExecTriggered->GetExecPin();
	UEdGraphPin* Node_SetSkipExecTriggered_Then = Node_SetSkipExecTriggered->GetThenPin();
	UEdGraphPin* Node_SetSkipExecTriggered_Variable = Node_SetSkipExecTriggered->GetVariablePin();
	UEdGraphPin* Node_SetSkipExecTriggered_Value = Node_SetSkipExecTriggered->GetValuePin();
	
	MAKE_NODE(TemporaryVariable, CancelExecTriggered, 0, 1250, Deferred);
	Node_CancelExecTriggered->VariableType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
	Node_CancelExecTriggered.FinishDeferredConstruction();

	MAKE_BASIC_NODE_OLD(SetCancelExecTriggered, UK2Node_AssignmentStatement, 250, 1250);
	UEdGraphPin* Node_SetCancelExecTriggered_Exec = Node_SetCancelExecTriggered->GetExecPin();
	UEdGraphPin* Node_SetCancelExecTriggered_Then = Node_SetCancelExecTriggered->GetThenPin();
	UEdGraphPin* Node_SetCancelExecTriggered_Variable = Node_SetCancelExecTriggered->GetVariablePin();
	UEdGraphPin* Node_SetCancelExecTriggered_Value = Node_SetCancelExecTriggered->GetValuePin();

	//MAKE_NODE(BangoLaunchSleep_Internal, LaunchSleep, 1000, 500);

	BangoK2NodeBuilder::BangoLaunchSleep_Internal Node_LaunchSleep(1000, 500);
	Node_LaunchSleep.Construct();

	MAKE_NODE(TemporaryVariable, ActionUUID, 0, 250, Deferred);
	Node_ActionUUID->VariableType.PinCategory = UEdGraphSchema_K2::PC_Int;
	Node_ActionUUID.FinishDeferredConstruction();
	
	MAKE_BASIC_NODE_OLD(SetUUID, UK2Node_AssignmentStatement, 1250, 500);
	UEdGraphPin* Node_SetUUID_Exec = Node_SetUUID->GetExecPin();
	UEdGraphPin* Node_SetUUID_Then = Node_SetUUID->GetThenPin();
	UEdGraphPin* Node_SetUUID_Variable = Node_SetUUID->GetVariablePin();
	UEdGraphPin* Node_SetUUID_Value = Node_SetUUID->GetValuePin();

	// FBlueprintEditorUtils::FindUniqueCustomEventName does not work. Generate my own unique ID.
	FGuid UniqueID = FGuid::NewGuid();
	
	MAKE_NODE(CustomEvent, TickEvent, 500, 2000, Deferred);
	Node_TickEvent->CustomFunctionName = FName(UniqueID.ToString(), 0);
	Node_TickEvent.FinishDeferredConstruction();
	UEdGraphPin* Node_TickEvent_Then = Node_TickEvent->GetThenPin();
	UEdGraphPin* Node_TickEvent_Delegate = Node_TickEvent->GetDelegatePin();

	MAKE_NODE(CustomEvent, CompleteEvent, 2500, 1000, Deferred);
	Node_CompleteEvent->CustomFunctionName = FName(UniqueID.ToString(), 1);
	Node_CompleteEvent.FinishDeferredConstruction();
	
	MAKE_NODE(Branch, CancelBranch, 1000, 500);
	
	MAKE_NODE(Branch, SkipBranch, 1000, 1000);
	
	MAKE_NODE(BooleanOR, CancelConditionsOR, 750, 500);
	
	MAKE_NODE(BooleanOR, SkipConditionsOR, 750, 1000);
	
	MAKE_NODE(BangoCancelSleep_Internal, CancelSleep, 1500, 500);
	
	MAKE_NODE(BangoSkipSleep_Internal, SkipSleep, 1500, 1000);
	
	// Set up default pin values
	Node_SetCancelExecTriggered_Value->DefaultValue = "True";
	Node_SetSkipExecTriggered_Value->DefaultValue = "True";
	
	// Wire up nodes

	// Launch Sleep inputs
	bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*Node_This_Exec, *Node_LaunchSleep.Exec).CanSafeConnect();
	bIsErrorFree &= Schema->TryCreateConnection(Node_TickEvent_Delegate, Node_LaunchSleep.TickDelegate);
	bIsErrorFree &= Schema->TryCreateConnection(Node_CompleteEvent.Delegate, Node_LaunchSleep.CompleteDelegate);

	if (Node_This_Duration->HasAnyConnections())
	{
		bIsErrorFree &= CompilerContext.CopyPinLinksToIntermediate(*Node_This_Duration, *Node_LaunchSleep.Duration).CanSafeConnect();
	}
	else
	{
		Node_LaunchSleep.Duration->DefaultValue = Node_This_Duration->DefaultValue;
	}
	
	// Set UUID inputs
	bIsErrorFree &= Schema->TryCreateConnection(Node_LaunchSleep.Then, Node_SetUUID_Exec);
	bIsErrorFree &= Schema->TryCreateConnection(Node_ActionUUID.Variable, Node_SetUUID_Variable);
	bIsErrorFree &= Schema->TryCreateConnection(Node_LaunchSleep.ReturnValue, Node_SetUUID_Value);

	// OR Cancel conditions inputs
	if (Node_This_CancelCondition)
	{
		bIsErrorFree &= CompilerContext.CopyPinLinksToIntermediate(*Node_This_CancelCondition, *Node_CancelConditionsOR.B).CanSafeConnect();	
	}

	// OR Skip conditions inputs
	if (Node_This_SkipCondition)
	{
		bIsErrorFree &= CompilerContext.CopyPinLinksToIntermediate(*Node_This_SkipCondition, *Node_SkipConditionsOR.B).CanSafeConnect();
	}

	// Branch Cancel inputs
	bIsErrorFree &= Schema->TryCreateConnection(Node_TickEvent_Then, Node_CancelBranch.Exec);
	bIsErrorFree &= Schema->TryCreateConnection(Node_CancelConditionsOR.Result, Node_CancelBranch.Condition);

	// Branch Skip inputs
	bIsErrorFree &= Schema->TryCreateConnection(Node_CancelBranch.Else, Node_SkipBranch.Exec);
	bIsErrorFree &= Schema->TryCreateConnection(Node_SkipConditionsOR.Result, Node_SkipBranch.Condition);

	// Cancel Sleep function inputs
	bIsErrorFree &= Schema->TryCreateConnection(Node_CancelBranch.Then, Node_CancelSleep.Exec);
	bIsErrorFree &= Schema->TryCreateConnection(Node_ActionUUID.Variable, Node_CancelSleep.ActionUUID);

	// Skip Sleep function inputs
	bIsErrorFree &= Schema->TryCreateConnection(Node_SkipBranch.Then, Node_SkipSleep.Exec);
	bIsErrorFree &= Schema->TryCreateConnection(Node_ActionUUID.Variable, Node_SkipSleep.ActionUUID);
		
	// Final output
	bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*Node_This_Completed, *Node_CompleteEvent.Then).CanSafeConnect();
	
	// Exec Skip/Cancel inputs
	if (Node_This_CancelExec)
	{
		bIsErrorFree &= Schema->TryCreateConnection(Node_CancelExecTriggered.Variable, Node_CancelConditionsOR.A);
		bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*Node_This_CancelExec, *Node_SetCancelExecTriggered_Exec).CanSafeConnect();
		bIsErrorFree &= Schema->TryCreateConnection(Node_CancelExecTriggered.Variable, Node_SetCancelExecTriggered_Variable);
	}
	if (Node_This_SkipExec)
	{
		bIsErrorFree &= Schema->TryCreateConnection(Node_SkipExecTriggered.Variable, Node_SkipConditionsOR.A);
		bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*Node_This_SkipExec, *Node_SetSkipExecTriggered_Exec).CanSafeConnect();
		bIsErrorFree &= Schema->TryCreateConnection(Node_SkipExecTriggered.Variable, Node_SetSkipExecTriggered_Variable);
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
