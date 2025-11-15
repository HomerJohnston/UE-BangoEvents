#include "BangoUncooked/BangoNodeBuilder.h"

#include "K2Node_AssignmentStatement.h"
#include "K2Node_CallFunction.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_AddDelegate.h"
#include "K2Node_CreateDelegate.h"
#include "K2Node_GenericCreateObject.h"
#include "K2Node_Self.h"
#include "K2Node_TemporaryVariable.h"
#include "KismetCompiler.h"
#include "Bango/Core/BangoScriptObject.h"
#include "BangoUncooked/K2/K2Node_BangoRunScript.h"
#include "BangoUncooked/K2/K2Node_BangoSleep.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

using namespace Bango_NodeBuilder;

using enum EBangoPinRequired;

// ==============================================
// New/Unsorted
// ==============================================

void SelfReference::Construct()
{
	AllocateDefaultPins();

	Self = FindPin("Self");
}

void IsValidPure::Construct()
{
	_Node->SetFromFunction(UKismetSystemLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UKismetSystemLibrary, IsValid)));
	AllocateDefaultPins();

	Target = FindPin("Self");
	Object = FindPin("Object");
	Result = _Node->GetReturnValuePin();
}

void AddDelegate::Construct()
{
	AllocateDefaultPins();

	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	Target = FindPin("Self");
	Delegate = _Node->GetDelegatePin();
}

void CreateDelegate::Construct()
{
	AllocateDefaultPins();

	ObjectIn = _Node->GetObjectInPin();
	DelegateOut = _Node->GetDelegateOutPin();
}

void CreateObject::Construct()
{
	AllocateDefaultPins();

	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	CreatedObject = _Node->GetResultPin();
	ObjectClass = _Node->GetClassPin();
}

void BangoRunScript::Construct()
{
	AllocateDefaultPins();

	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	Completed = FindPin("Completed");
	Script = _Node->GetScriptPin();
	Handle = FindPin("ReturnValue");
}

void BangoExecuteScript_Internal::Construct()
{
	_Node->SetFromFunction(UBangoScriptObject::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoScriptObject, Execute_Internal)));
	AllocateDefaultPins();

	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	Target = FindPin("Self");
	Result = _Node->GetReturnValuePin();
}

// ==============================================
// Math nodes
// ==============================================

// ==============================================
// Boolean logic nodes
// ==============================================

// ==============================================
// Engine nodes
// ==============================================

// ==============================================
// Variable operations
// ==============================================

// ==============================================
// Bango nodes
// ==============================================

void AssignmentStatement::Construct()
{
	AllocateDefaultPins();

	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	Variable = _Node->GetVariablePin();
	Value = _Node->GetValuePin();
}

// ----------------------------------------------

void BooleanOR::Construct()
{
	_Node->SetFromFunction(UKismetMathLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary, BooleanOR)));
	AllocateDefaultPins();
	
	A = FindPin("A");
	B = FindPin("B");
	Result = _Node->GetReturnValuePin();
}

// ----------------------------------------------

void Branch::Construct()
{
	AllocateDefaultPins();
	
	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	Condition = _Node->GetConditionPin();
	Else = _Node->GetElsePin();
}

// ----------------------------------------------

void CustomEvent::Construct()
{
	AllocateDefaultPins();
	
	Then = _Node->GetThenPin();
	Delegate = _Node->GetDelegatePin();
}

// ----------------------------------------------

void TemporaryVariable::Construct()
{
	AllocateDefaultPins();
	
	Variable = _Node->GetVariablePin();
}

// ----------------------------------------------

void BangoCancelSleep_Internal::Construct()
{
	_Node->SetFromFunction(UBangoScriptObject::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoScriptObject, CancelSleep_Internal)));
	AllocateDefaultPins();

	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	ActionUUID = FindPin("ActionUUID");
}

// ----------------------------------------------

void BangoLaunchSleep_Internal::Construct()
{
	_Node->SetFromFunction(UBangoScriptObject::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoScriptObject, LaunchSleep_Internal)));
	AllocateDefaultPins();

	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	Duration = FindPin("Duration");
	TickDelegate = FindPin("TickDelegate");
	CompleteDelegate = FindPin("CompleteDelegate");
	ReturnValue = _Node->GetReturnValuePin();
}

// ----------------------------------------------

void BangoSkipSleep_Internal::Construct()
{
	_Node->SetFromFunction(UBangoScriptObject::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoScriptObject, SkipSleep_Internal)));
	AllocateDefaultPins();

	Exec = _Node->GetExecPin();
	Then = _Node->GetThenPin();
	ActionUUID = FindPin("ActionUUID");
}

// ----------------------------------------------

void BangoSleep::Construct()
{
	AllocateDefaultPins();
	
	Exec = _Node->GetExecPin();
	Duration = FindPin("Duration");
	SkipExec = FindPin("SkipExec", Optional);
	CancelExec = FindPin("CancelExec", Optional);
	SkipCondition = FindPin("SkipCondition", Optional);
	CancelCondition = FindPin("CancelCondition", Optional);
	Completed = FindPin("Completed");
}
