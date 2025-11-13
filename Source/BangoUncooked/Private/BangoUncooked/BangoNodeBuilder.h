#pragma once

#include "K2Node.h"
#include "KismetCompiler.h"

#include "BangoNodeBuilder_Macros.h"

class UK2Node_TemporaryVariable;
class UK2Node_CustomEvent;
class UK2Node_IfThenElse;
class UK2Node_CallFunction;

using enum EBangoDeferConstruction;

namespace BangoK2NodeBuilder
{
	// ------------------------------------------
	// Special engine nodes
	
	MAKE_NODE_TYPE(TemporaryVariable, UK2Node_TemporaryVariable,
		*Variable);
	
	MAKE_NODE_TYPE(CustomEvent, UK2Node_CustomEvent,
		*Then, *Delegate);
	
	MAKE_NODE_TYPE(Branch, UK2Node_IfThenElse,
		*Exec, *Then, *Condition, *Else);
	
	// ------------------------------------------
	// Generic CallFunction engine nodes
	
	MAKE_NODE_TYPE(BooleanOR, UK2Node_CallFunction,
		*A, *B, *Result);
	
	// ------------------------------------------
	// Bango nodes

	MAKE_NODE_TYPE(BangoLaunchSleep_Internal, UK2Node_CallFunction,
		*Exec, *Then, *ReturnValue, *Duration, *TickDelegate, *CompleteDelegate);
	
	MAKE_NODE_TYPE(BangoCancelSleep_Internal, UK2Node_CallFunction,
		*Exec, *Then, *ActionUUID);
	
	MAKE_NODE_TYPE(BangoSkipSleep_Internal, UK2Node_CallFunction,
		*Exec, *Then, *ActionUUID);
};
