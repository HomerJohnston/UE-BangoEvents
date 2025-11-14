#include "BangoUncooked/BangoNodeBuilder.h"

#include "K2Node_AssignmentStatement.h"
#include "K2Node_CallFunction.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_TemporaryVariable.h"
#include "KismetCompiler.h"
#include "Bango/Core/BangoScriptObject.h"
#include "Kismet/KismetMathLibrary.h"

using namespace Bango_BuildNode;

// ----------------------------------------------
// ----------------------------------------------
// ----------------------------------------------
// ----------------------------------------------
// ----------------------------------------------
// ----------------------------------------------
// ----------------------------------------------
// ----------------------------------------------
void TemporaryVariable::Construct()
{
	// I need a way to inject logic before this step from the ExpandNode side
	Node->AllocateDefaultPins();
	
	Variable = Node->GetVariablePin();
}

// ----------------------------------------------

void CustomEvent::Construct()
{
	Node->AllocateDefaultPins();
	
	Then = Node->GetThenPin();
	Delegate = Node->GetDelegatePin();
}

// ----------------------------------------------

void BooleanOR::Construct()
{
	Node->SetFromFunction(UKismetMathLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary, BooleanOR)));
	Node->AllocateDefaultPins();
	
	A = Node->FindPin(TEXT("A"));
	B = Node->FindPin(TEXT("B"));
	Result = Node->GetReturnValuePin();
}

// ----------------------------------------------

void AssignmentStatement::Construct()
{
	Node->AllocateDefaultPins();

	Exec = Node->GetExecPin();
	Then = Node->GetThenPin();
	Variable = Node->GetVariablePin();
	Value = Node->GetValuePin();
}

// ----------------------------------------------

void Branch::Construct()
{
	Node->AllocateDefaultPins();
	
	Exec = Node->GetExecPin();
	Then = Node->GetThenPin();
	Condition = Node->GetConditionPin();
	Else = Node->GetElsePin();
}

// ----------------------------------------------

void BangoCancelSleep_Internal::Construct()
{
	Node->SetFromFunction(UBangoScriptObject::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoScriptObject, CancelSleep_Internal)));
	Node->AllocateDefaultPins();

	Exec = Node->GetExecPin();
	Then = Node->GetThenPin();
	ActionUUID = FindPin("ActionUUID");
}

// ----------------------------------------------

void BangoLaunchSleep_Internal::Construct()
{
	Node->SetFromFunction(UBangoScriptObject::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoScriptObject, LaunchSleep_Internal)));
	Node->AllocateDefaultPins();

	Exec = Node->GetExecPin();
	Then = Node->GetThenPin();
	Duration = FindPin("Duration");
	TickDelegate = FindPin("TickDelegate");
	CompleteDelegate = FindPin("CompleteDelegate");
	ReturnValue = Node->GetReturnValuePin();
}


void BangoSkipSleep_Internal::Construct()
{
	Node->SetFromFunction(UBangoScriptObject::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoScriptObject, SkipSleep_Internal)));
	Node->AllocateDefaultPins();

	Exec = Node->GetExecPin();
	Then = Node->GetThenPin();
	ActionUUID = FindPin("ActionUUID");
}
