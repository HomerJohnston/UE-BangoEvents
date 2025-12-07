#pragma once

#include "K2Node.h"
#include "KismetCompiler.h"

#include "BangoUncooked/Private/BangoUncooked/NodeBuilder/BangoNodeBuilder_Macros.h"

#include "K2Node_AssignmentStatement.h"
#include "K2Node_CallFunction.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_AddDelegate.h"
#include "K2Node_DynamicCast.h"
#include "K2Node_CreateDelegate.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_GenericCreateObject.h"
#include "K2Node_Self.h"
#include "K2Node_TemporaryVariable.h"
#include "KismetCompiler.h"
#include "Bango/Core/BangoScriptObject.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoRunScript.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoSleep.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Bango/Subsystem/BangoActorIDSubsystem.h"
#include "Bango/Subsystem/BangoActorIDSubsystem.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoFindActor.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoGotoDestination.h"
#include "K2Node_Knot.h"

using namespace BangoNodeBuilder;	
using enum EBangoPinRequired;

// ==========================================
MAKE_NODE_TYPE(SelfReference, UK2Node_Self, NORMAL_CONSTRUCTION, Self);

inline void SelfReference::Construct()
{
	AllocateDefaultPins();
	Self = FindPin("Self");
}

// ==========================================
MAKE_NODE_TYPE(Knot, UK2Node_Knot, NORMAL_CONSTRUCTION, InputPin, OutputPin);

inline void Knot::Construct()
{
	AllocateDefaultPins();
	InputPin = FindPin("InputPin");
	OutputPin = FindPin("OutputPin");
}

// ==========================================
MAKE_NODE_TYPE(IsValidPure, UK2Node_CallFunction, NORMAL_CONSTRUCTION, Target, Object, Result);

inline void IsValidPure::Construct()
{
	_Node->SetFromFunction(UKismetSystemLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UKismetSystemLibrary, IsValid)));
	AllocateDefaultPins();

	Target = FindPin("Self");
	Object = FindPin("Object");
	Result = _Node->GetReturnValuePin();
}

// ==========================================
MAKE_NODE_TYPE(AddDelegate, UK2Node_AddDelegate, DEFERRED_CONSTRUCTION,	Exec, Then, Target, Delegate);

inline void AddDelegate::Construct()
{
	AllocateDefaultPins();

	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	Target = FindPin("Self");
	Delegate = _Node->GetDelegatePin();
}

// ==========================================
MAKE_NODE_TYPE(CreateDelegate, UK2Node_CreateDelegate, NORMAL_CONSTRUCTION,	ObjectIn, DelegateOut);

inline void CreateDelegate::Construct()
{
	AllocateDefaultPins();
	ObjectIn = _Node->GetObjectInPin();
	DelegateOut = _Node->GetDelegateOutPin();
}

// ==========================================
MAKE_NODE_TYPE(BooleanOR, UK2Node_CallFunction, NORMAL_CONSTRUCTION, A, B, Result);

inline void BooleanOR::Construct()
{
	_Node->SetFromFunction(UKismetMathLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary, BooleanOR)));
	AllocateDefaultPins();
	A = FindPin("A");
	B = FindPin("B");
	Result = _Node->GetReturnValuePin();
}

// ==========================================
MAKE_NODE_TYPE(DynamicCast_Pure, UK2Node_DynamicCast, DEFERRED_CONSTRUCTION, ObjectToCast, CastedObject, Success);

inline void DynamicCast_Pure::Construct()
{
	AllocateDefaultPins();
	_Node->SetPurity(true);
	ObjectToCast = _Node->GetCastSourcePin();
	CastedObject = _Node->GetCastResultPin();
	Success = _Node->GetBoolSuccessPin();
}

// ==========================================
MAKE_NODE_TYPE(CreateObject, UK2Node_GenericCreateObject, NORMAL_CONSTRUCTION, Exec, Then, CreatedObject, ObjectClass);

inline void CreateObject::Construct()
{
	AllocateDefaultPins();
	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	CreatedObject = _Node->GetResultPin();
	ObjectClass = _Node->GetClassPin();
}

// ==========================================
MAKE_NODE_TYPE(CallFunction, UK2Node_CallFunction, DEFERRED_CONSTRUCTION, Exec, Then, Target);

inline void CallFunction::Construct()
{
	AllocateDefaultPins();
	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	Target = FindPin("Self");
}

// ==========================================
MAKE_NODE_TYPE(Branch, UK2Node_IfThenElse, NORMAL_CONSTRUCTION, Exec, Then, Condition, Else);

inline void Branch::Construct()
{
	AllocateDefaultPins();
	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	Condition = _Node->GetConditionPin();
	Else = _Node->GetElsePin();
}

// ==========================================
MAKE_NODE_TYPE(CustomEvent, UK2Node_CustomEvent, NORMAL_CONSTRUCTION, Then, Delegate);
		
inline void CustomEvent::Construct()
{
	AllocateDefaultPins();
	Then = _Node->GetThenPin();
	Delegate = _Node->GetDelegatePin();
}

// ==========================================
MAKE_NODE_TYPE(TemporaryVariable, UK2Node_TemporaryVariable, DEFERRED_CONSTRUCTION, Variable);

inline void TemporaryVariable::Construct()
{
	AllocateDefaultPins();
	Variable = _Node->GetVariablePin();
}

// ==========================================
MAKE_NODE_TYPE(AssignmentStatement, UK2Node_AssignmentStatement, NORMAL_CONSTRUCTION, Exec, Then, Variable, Value);

inline void AssignmentStatement::Construct()
{
	AllocateDefaultPins();
	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	Variable = _Node->GetVariablePin();
	Value = _Node->GetValuePin();
}

// ==========================================
MAKE_NODE_TYPE(ExecutionSequence, UK2Node_ExecutionSequence, NORMAL_CONSTRUCTION, Exec);

inline void ExecutionSequence::Construct()
{
	AllocateDefaultPins();
	Exec = _Node->GetExecPin();
}

// ==========================================
MAKE_NODE_TYPE(BangoRunScript, UK2Node_BangoRunScript, NORMAL_CONSTRUCTION, Exec, Then, Completed, Script, Handle);

inline void BangoRunScript::Construct()
{
	AllocateDefaultPins();
	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	Completed = FindPin("Completed");
	Script = _Node->GetScriptPin();
	Handle = FindPin("ReturnValue");
}

// ==========================================
MAKE_NODE_TYPE(BangoExecuteScript_Internal, UK2Node_CallFunction, NORMAL_CONSTRUCTION, Exec, Then, Target, Result);

inline void BangoExecuteScript_Internal::Construct()
{
	_Node->SetFromFunction(UBangoScriptInstance::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoScriptInstance, Execute_Internal)));
	AllocateDefaultPins();
	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	Target = FindPin("Self");
	Result = _Node->GetReturnValuePin();
}

// ==========================================
MAKE_NODE_TYPE(BangoCancelSleep_Internal, UK2Node_CallFunction, NORMAL_CONSTRUCTION, Exec, Then, ActionUUID);

inline void BangoCancelSleep_Internal::Construct()
{
	_Node->SetFromFunction(UBangoScriptInstance::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoScriptInstance, CancelSleep_Internal)));
	AllocateDefaultPins();
	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	ActionUUID = FindPin("ActionUUID");
}

// ==========================================
MAKE_NODE_TYPE(BangoLaunchSleep_Internal, UK2Node_CallFunction, NORMAL_CONSTRUCTION, Exec, Then, ReturnValue, Duration, TickDelegate, CompleteDelegate);

inline void BangoLaunchSleep_Internal::Construct()
{
	_Node->SetFromFunction(UBangoScriptInstance::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoScriptInstance, LaunchSleep_Internal)));
	AllocateDefaultPins();
	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	Duration = FindPin("Duration");
	TickDelegate = FindPin("TickDelegate");
	CompleteDelegate = FindPin("CompleteDelegate");
	ReturnValue = _Node->GetReturnValuePin();
}

// ==========================================
MAKE_NODE_TYPE(BangoSkipSleep_Internal, UK2Node_CallFunction, NORMAL_CONSTRUCTION, Exec, Then, ActionUUID);

inline void BangoSkipSleep_Internal::Construct()
{
	_Node->SetFromFunction(UBangoScriptInstance::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoScriptInstance, SkipSleep_Internal)));
	AllocateDefaultPins();
	Exec = _Node->GetExecPin();
	ActionUUID = FindPin("ActionUUID");
	Then = _Node->GetThenPin();
}

// ==========================================
MAKE_NODE_TYPE(BangoPauseSleep_Internal, UK2Node_CallFunction, NORMAL_CONSTRUCTION, Exec, Then, Paused, ActionUUID);

inline void BangoPauseSleep_Internal::Construct()
{
	_Node->SetFromFunction(UBangoScriptInstance::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoScriptInstance, SetSleepPause_Internal)));
	AllocateDefaultPins();
	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	Paused = FindPin("bPaused");
	ActionUUID = FindPin("ActionUUID");
}

// ==========================================
MAKE_NODE_TYPE(BangoSleep, UK2Node_BangoSleep, NORMAL_CONSTRUCTION, Exec, Duration, InfiniteDuration, SkipExec, CancelExec, SkipCondition, CancelCondition, PauseCondition, Completed);

inline void BangoSleep::Construct()
{
	AllocateDefaultPins();
	Exec = _Node->GetExecPin();
	Duration = FindPin("Duration", Optional);
	InfiniteDuration = FindPin("InfiniteDuration", Optional);
	SkipExec = FindPin("SkipExec", Optional);
	CancelExec = FindPin("CancelExec", Optional);
	SkipCondition = FindPin("SkipCondition", Optional);
	CancelCondition = FindPin("CancelCondition", Optional);
	PauseCondition = FindPin("PauseCondition", Optional);
	Completed = FindPin("Completed");
}

// ==========================================
MAKE_NODE_TYPE(BangoFindActor, UK2Node_BangoFindActor, NORMAL_CONSTRUCTION, ActorID, FoundActor);

inline void BangoFindActor::Construct()
{
	AllocateDefaultPins();
	ActorID = FindPin("ActorID");
	FoundActor = FindPin("FoundActor");
}