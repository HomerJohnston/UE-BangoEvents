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
	static const FName PauseCondition(TEXT("PauseCondition"));
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

	if (bEnablePausePin)
	{
		UEdGraphPin* PauseConditionPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, K2Node_BangoSleepPins::PauseCondition);
		PauseConditionPin->PinFriendlyName = LOCTEXT("SleepNodePauseConditionPinLabel", "Pause");
		PauseConditionPin->PinToolTip = "Polled on tick. If true, the sleep timer will pause. Will still react to cancel/skip!";
	}
}

void UK2Node_BangoSleep::ExpandNode(class FKismetCompilerContext& Compiler, UEdGraph* SourceGraph)
{
	Super::ExpandNode(Compiler, SourceGraph);

	const UEdGraphSchema_K2* Schema = Compiler.GetSchema();
	bool bIsErrorFree = true;

	// FBlueprintEditorUtils::FindUniqueCustomEventName does not work. Generate my own unique ID.
	FString UniqueID = *Compiler.GetGuid(this);

	check(this);
	check(SourceGraph);
	//Bango_NodeBuilder::Setup(Compiler, SourceGraph, this, Schema, &bIsErrorFree, FVector2f(5000, 5000));
	Bango_NodeBuilder::Builder Builder(Compiler, SourceGraph, this, Schema, &bIsErrorFree, FVector2f(5000, 5000));
	
	// -----------------
	// Make nodes
	
	using namespace Bango_NodeBuilder;
	auto Node_SkipExecTriggered =		Builder.MakeNode<TemporaryVariable>(0, 1);
	auto Node_CancelExecTriggered =		Builder.MakeNode<TemporaryVariable>(0, 10);
	auto Node_ActionUUID =				Builder.MakeNode<TemporaryVariable>(0, 2);
	auto Node_SetSkipExecTriggered =	Builder.MakeNode<AssignmentStatement>(0, 1);
	auto Node_SetCancelExecTriggered =	Builder.MakeNode<AssignmentStatement>(2, 1);
	auto Node_SetUUID =					Builder.MakeNode<AssignmentStatement>(1, 1);
	auto Node_LaunchSleep = 			Builder.MakeNode<BangoLaunchSleep_Internal>(2, 1);
	auto Node_TickEvent = 				Builder.MakeNode<CustomEvent>(2, 4); 
	auto Node_CompleteEvent = 			Builder.MakeNode<CustomEvent>(5, 2);
	auto Node_CancelBranch = 			Builder.MakeNode<Branch>(4, 1);
	auto Node_SkipBranch = 				Builder.MakeNode<Branch>(4, 4);
	auto Node_CancelConditionsOR = 		Builder.MakeNode<BooleanOR>(750, 500);
	auto Node_SkipConditionsOR = 		Builder.MakeNode<BooleanOR>(750, 1000);
	auto Node_CancelSleep = 			Builder.MakeNode<BangoCancelSleep_Internal>(1500, 500);
	auto Node_SkipSleep = 				Builder.MakeNode<BangoSkipSleep_Internal>(1500, 1000);
	auto Node_This =					Builder.WrapExistingNode<BangoSleep>(this);
	UE_LOG(LogBango, Display, TEXT("Test"));

	// -----------------
	// Post-setup
	
#if 0
	Node_SkipExecTriggered->VariableType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
	Node_CancelExecTriggered->VariableType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
	Node_ActionUUID->VariableType.PinCategory = UEdGraphSchema_K2::PC_Int;

	Node_SetSkipExecTriggered.Value->DefaultValue = "True";
	Node_SetCancelExecTriggered.Value->DefaultValue = "True";
	
	Node_TickEvent->CustomFunctionName = FName(UniqueID, 0);
	Node_CompleteEvent->CustomFunctionName = FName(UniqueID, 1); 
	
	Bango_NodeBuilder::FinishSpawningAllNodes();
	
	// -----------------
	// Make connections
	
	// Launch Sleep inputs

	/*
	MoveExternalConnection(Node_This.Exec, Node_LaunchSleep.Exec);
	CreateConnection(Node_TickEvent.Delegate, Node_LaunchSleep.TickDelegate);
	CreateConnection(Node_CompleteEvent.Delegate, Node_LaunchSleep.CompleteDelegate);
	
	if (Node_This.Duration->HasAnyConnections())
	{
		CopyExternalConnection(Node_This.Duration, Node_LaunchSleep.Duration);
	}
	else
	{
		SetDefaultValue(Node_LaunchSleep.Duration, Node_This.Duration->DefaultValue);
	}
	
	// Set UUID inputs
	CreateConnection(Node_LaunchSleep.Then, Node_SetUUID.Exec);
	CreateConnection(Node_ActionUUID.Variable, Node_SetUUID.Variable);
	CreateConnection(Node_LaunchSleep.ReturnValue, Node_SetUUID.Value);
	
	// OR Cancel conditions inputs
	if (Node_This.CancelCondition)
	{
		CopyExternalConnection(Node_This.CancelCondition, Node_CancelConditionsOR.B);
	}

	// OR Skip conditions inputs
	if (Node_This.SkipCondition)
	{
		CopyExternalConnection(Node_This.SkipCondition, Node_SkipConditionsOR.B);
	}

	// Branch Cancel inputs
	CreateConnection(Node_TickEvent.Then, Node_CancelBranch.Exec);
	CreateConnection(Node_CancelConditionsOR.Result, Node_CancelBranch.Condition);

	// Branch Skip inputs
	CreateConnection(Node_CancelBranch.Else, Node_SkipBranch.Exec);
	CreateConnection(Node_SkipConditionsOR.Result, Node_SkipBranch.Condition);

	// Cancel Sleep function inputs
	CreateConnection(Node_CancelBranch.Then, Node_CancelSleep.Exec);
	CreateConnection(Node_ActionUUID.Variable, Node_CancelSleep.ActionUUID);

	// Skip Sleep function inputs
	CreateConnection(Node_SkipBranch.Then, Node_SkipSleep.Exec);
	CreateConnection(Node_ActionUUID.Variable, Node_SkipSleep.ActionUUID);
		
	// Final output
	MoveExternalConnection(Node_This.Completed, Node_CompleteEvent.Then);
	
	// Exec Skip/Cancel inputs
	if (Node_This.CancelExec)
	{
		CreateConnection(Node_CancelExecTriggered.Variable, Node_CancelConditionsOR.A);
		MoveExternalConnection(Node_This.CancelExec, Node_SetCancelExecTriggered.Exec);
		CreateConnection(Node_CancelExecTriggered.Variable, Node_SetCancelExecTriggered.Variable);
	}
	if (Node_This.SkipExec)
	{
		CreateConnection(Node_SkipExecTriggered.Variable, Node_SkipConditionsOR.A);
		MoveExternalConnection(Node_This.SkipExec, Node_SetSkipExecTriggered.Exec);
		CreateConnection(Node_SkipExecTriggered.Variable, Node_SetSkipExecTriggered.Variable);
	}
	*/

	Bango_NodeBuilder::FinishSpawningAllNodes();
#endif
	
	// Done!
	if (!bIsErrorFree)
	{
		Compiler.MessageLog.Error(*LOCTEXT("InternalConnectionError", "K2Node_LoadAsset: Internal connection error. @@").ToString(), this);
	}
	
	// Disconnect ThisNode from the graph
	BreakAllNodeLinks();
}

#undef LOCTEXT_NAMESPACE
