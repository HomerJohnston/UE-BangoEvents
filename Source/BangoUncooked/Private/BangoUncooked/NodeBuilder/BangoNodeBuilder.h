#pragma once

#include "K2Node.h"
#include "KismetCompiler.h"

#include "BangoNodeBuilder_Macros.h"

class UK2Node_BangoFindActor;
class UK2Node_BangoRunScript;
class UK2Node_AddDelegate;
class UK2Node_BangoSleep;
class UK2Node_AssignmentStatement;
class UK2Node_TemporaryVariable;
class UK2Node_CustomEvent;
class UK2Node_IfThenElse;
class UK2Node_CallFunction;
class UK2Node_GenericCreateObject;
class UK2Node_Self;

// NODES SHOULD BE SORTED ALPHABETICALLY

namespace Bango_NodeBuilder
{
	// ------------------------------------------
	// Misc.

	MAKE_NODE_TYPE(SelfReference, UK2Node_Self, NORMAL_CONSTRUCTION,
		Self);
	
	MAKE_NODE_TYPE(IsValidPure, UK2Node_CallFunction, NORMAL_CONSTRUCTION,
		Target, Object, Result);

	
	MAKE_NODE_TYPE(AddDelegate, UK2Node_AddDelegate, DEFERRED_CONSTRUCTION,
		Exec, Then, Target, Delegate);
	
	// ------------------------------------------
	// Delegate operations
	
	MAKE_NODE_TYPE(CreateDelegate, UK2Node_CreateDelegate, NORMAL_CONSTRUCTION,
		ObjectIn, DelegateOut);
	
	// ------------------------------------------
	// Math nodes
	
	
	// ------------------------------------------
	// Boolean logic nodes

	MAKE_NODE_TYPE(BooleanOR, UK2Node_CallFunction, NORMAL_CONSTRUCTION,
		A, B, Result);
	
	// ------------------------------------------
	// Engine nodes

	// Construct Object from Class
	MAKE_NODE_TYPE(CreateObject, UK2Node_GenericCreateObject, NORMAL_CONSTRUCTION,
		Exec, Then, CreatedObject, ObjectClass);

	MAKE_NODE_TYPE(CallFunction, UK2Node_CallFunction, DEFERRED_CONSTRUCTION,
		Exec, Then, Target);
	
	// ------------------------------------------
	// Variable operations
	
	MAKE_NODE_TYPE(Branch, UK2Node_IfThenElse, NORMAL_CONSTRUCTION,
		Exec, Then, Condition, Else);
	
	MAKE_NODE_TYPE(CustomEvent, UK2Node_CustomEvent, NORMAL_CONSTRUCTION,
		Then, Delegate);
			
	MAKE_NODE_TYPE(TemporaryVariable, UK2Node_TemporaryVariable, DEFERRED_CONSTRUCTION,
		Variable);
	
	MAKE_NODE_TYPE(AssignmentStatement, UK2Node_AssignmentStatement, NORMAL_CONSTRUCTION,
		Exec, Then, Variable, Value);
	
	// ------------------------------------------
	// Bango nodes

	// Run Script
	MAKE_NODE_TYPE(BangoRunScript, UK2Node_BangoRunScript, NORMAL_CONSTRUCTION,
		Exec, Then, Completed, Script, Handle);
	
	MAKE_NODE_TYPE(BangoExecuteScript_Internal, UK2Node_CallFunction, NORMAL_CONSTRUCTION,
		Exec, Then, Target, Result);
	
	// Sleep
	MAKE_NODE_TYPE(BangoCancelSleep_Internal, UK2Node_CallFunction, NORMAL_CONSTRUCTION,
		Exec, Then, ActionUUID);

	MAKE_NODE_TYPE(BangoLaunchSleep_Internal, UK2Node_CallFunction, NORMAL_CONSTRUCTION,
		Exec, Then, ReturnValue, Duration, TickDelegate, CompleteDelegate);

	MAKE_NODE_TYPE(BangoSkipSleep_Internal, UK2Node_CallFunction, NORMAL_CONSTRUCTION,
		Exec, Then, ActionUUID);
	
	MAKE_NODE_TYPE(BangoPauseSleep_Internal, UK2Node_CallFunction, NORMAL_CONSTRUCTION,
		Exec, Then, Paused, ActionUUID);
	
	MAKE_NODE_TYPE(BangoSleep, UK2Node_BangoSleep, NORMAL_CONSTRUCTION,
		Exec, Duration, InfiniteDuration, SkipExec, CancelExec, SkipCondition, CancelCondition, PauseCondition, Completed);
		
	MAKE_NODE_TYPE(BangoFindActor, UK2Node_BangoFindActor, NORMAL_CONSTRUCTION,
		ActorID, FoundActor);
};
