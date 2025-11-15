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
	AllocatePins();

	Self = FindPin("Self");
}

void IsValidPure::Construct()
{
	Node->SetFromFunction(UKismetSystemLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UKismetSystemLibrary, IsValid)));
	AllocatePins();

	Target = FindPin("Self");
	Object = FindPin("Object");
	Result = Node->GetReturnValuePin();
}

void AddDelegate::Construct()
{
	AllocatePins();

	Exec = Node->GetExecPin();
	Then = Node->GetThenPin();
	Target = FindPin("Self");
	Delegate = Node->GetDelegatePin();
}

void CreateDelegate::Construct()
{
	AllocatePins();

	ObjectIn = Node->GetObjectInPin();
	DelegateOut = Node->GetDelegateOutPin();
}

void CreateObject::Construct()
{
	AllocatePins();

	Exec = Node->GetExecPin();
	Then = Node->GetThenPin();
	CreatedObject = Node->GetResultPin();
	ObjectClass = Node->GetClassPin();
}

void BangoRunScript::Construct()
{
	AllocatePins();

	Exec = Node->GetExecPin();
	Then = Node->GetThenPin();
	Completed = FindPin("Completed");
	Script = Node->GetScriptPin();
	Handle = FindPin("ReturnValue");
}

void BangoExecuteScript_Internal::Construct()
{
	Node->SetFromFunction(UBangoScriptObject::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoScriptObject, Execute_Internal)));
	AllocatePins();

	Exec = Node->GetExecPin();
	Then = Node->GetThenPin();
	Target = FindPin("Self");
	Result = Node->GetReturnValuePin();
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
	AllocatePins();

	Exec = Node->GetExecPin();
	Then = Node->GetThenPin();
	Variable = Node->GetVariablePin();
	Value = Node->GetValuePin();
}

// ----------------------------------------------

void BooleanOR::Construct()
{
	Node->SetFromFunction(UKismetMathLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary, BooleanOR)));
	AllocatePins();
	
	A = FindPin("A");
	B = FindPin("B");
	Result = Node->GetReturnValuePin();
}

// ----------------------------------------------

void Branch::Construct()
{
	AllocatePins();
	
	Exec = Node->GetExecPin();
	Then = Node->GetThenPin();
	Condition = Node->GetConditionPin();
	Else = Node->GetElsePin();
}

// ----------------------------------------------

void CustomEvent::Construct()
{
	AllocatePins();
	
	Then = Node->GetThenPin();
	Delegate = Node->GetDelegatePin();
}

// ----------------------------------------------

void TemporaryVariable::Construct()
{
	AllocatePins();
	
	Variable = Node->GetVariablePin();
}

// ----------------------------------------------

void BangoCancelSleep_Internal::Construct()
{
	Node->SetFromFunction(UBangoScriptObject::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoScriptObject, CancelSleep_Internal)));
	AllocatePins();

	Exec = Node->GetExecPin();
	Then = Node->GetThenPin();
	ActionUUID = FindPin("ActionUUID");
}

// ----------------------------------------------

void BangoLaunchSleep_Internal::Construct()
{
	Node->SetFromFunction(UBangoScriptObject::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoScriptObject, LaunchSleep_Internal)));
	AllocatePins();

	Exec = Node->GetExecPin();
	Then = Node->GetThenPin();
	Duration = FindPin("Duration");
	TickDelegate = FindPin("TickDelegate");
	CompleteDelegate = FindPin("CompleteDelegate");
	ReturnValue = Node->GetReturnValuePin();
}

// ----------------------------------------------

void BangoSkipSleep_Internal::Construct()
{
	Node->SetFromFunction(UBangoScriptObject::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoScriptObject, SkipSleep_Internal)));
	AllocatePins();

	Exec = Node->GetExecPin();
	Then = Node->GetThenPin();
	ActionUUID = FindPin("ActionUUID");
}

// ----------------------------------------------

void BangoSleep::Construct()
{
	AllocatePins();
	
	Exec = Node->GetExecPin();
	Duration = FindPin("Duration");
	SkipExec = FindPin("SkipExec", Optional);
	CancelExec = FindPin("CancelExec", Optional);
	SkipCondition = FindPin("SkipCondition", Optional);
	CancelCondition = FindPin("CancelCondition", Optional);
	Completed = FindPin("Completed");
}
