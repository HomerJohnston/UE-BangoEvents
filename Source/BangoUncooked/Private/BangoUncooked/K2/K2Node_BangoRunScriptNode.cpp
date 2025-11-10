#include "K2Node_BangoRunScriptNode.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_AddDelegate.h"
#include "K2Node_CallFunction.h"
#include "K2Node_ConstructObjectFromClass.h"
#include "K2Node_CreateDelegate.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_GenericCreateObject.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_Self.h"
#include "K2Node_TemporaryVariable.h"
#include "KismetCompiler.h"
#include "Bango/Core/BangoBlueprintFunctionLibrary.h"
#include "Bango/Core/BangoScriptObject.h"
#include "Bango/K2/BangoRunScriptNode.h"
#include "Kismet/KismetSystemLibrary.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

#define TRY_PARSE_PIN_TYPE(TYPE)\
	if (!bAddPin && PropertyClass == F##TYPE##Property::StaticClass())\
	{\
		bAddPin = true;\
		PinTypeName = UEdGraphSchema_K2::PC_##TYPE;\
	}
            
#define TRY_PARSE_PIN_TYPE2(TYPE, TYPE2)\
	if (!bAddPin && PropertyClass == F##TYPE##Property::StaticClass())\
	{\
		bAddPin = true;\
		PinTypeName = UEdGraphSchema_K2::PC_##TYPE2;\
	}

class UK2Node_IfThenElse;

UK2Node_BangoRunScript::UK2Node_BangoRunScript()
{
    //ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UBangoRunScript, RunScript);
   // ProxyFactoryClass = UBangoRunScript::StaticClass();
   // ProxyClass = UBangoRunScript::StaticClass();
   // ProxyActivateFunctionName = GET_FUNCTION_NAME_CHECKED(UBangoRunScript, Activate);
}

void UK2Node_BangoRunScript::PinDefaultValueChanged(UEdGraphPin* Pin)
{
    const UEdGraphPin* ScriptPin = GetScriptPin();
    
    if (Pin == ScriptPin)
    {
    	UpdateScriptPins(Pin->DefaultObject);
    }
}

void UK2Node_BangoRunScript::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
    const FName PropertyName = (PropertyChangedEvent.Property  ? PropertyChangedEvent.Property->GetFName() : NAME_None);
    if (PropertyName == GET_MEMBER_NAME_CHECKED(UK2Node_BangoRunScript, PinNames))
    {
        ReconstructNode();
    }
    
    Super::PostEditChangeProperty(PropertyChangedEvent);
    
    GetGraph()->NotifyNodeChanged(this);
}

void UK2Node_BangoRunScript::PostLoad()
{
	Super::PostLoad();
}

void UK2Node_BangoRunScript::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	AllocateDefaultPins();
	
	UEdGraphPin* ScriptPin = GetScriptPin(&OldPins);
	if (ScriptPin && ScriptPin->DefaultObject)
	{
		UpdateScriptPins(ScriptPin->DefaultObject);
	}

	RestoreSplitPins(OldPins);
}

void UK2Node_BangoRunScript::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (Pin && Pin == GetScriptPin())
	{
		UpdateScriptPins(Pin->DefaultObject);
	}
}

FText UK2Node_BangoRunScript::GetMenuCategory() const
{
	return LOCTEXT("RunScript_MenuCategory", "Bango|Test");
}

// Create a node, set up default pins, and get automatic references to its Exec and Then pins.
#define MAKE_NODE(NodeName, NodeType)\
	auto* const NodeName = CompilerContext.SpawnIntermediateNode<NodeType>(this, SourceGraph);\

#define MAKE_NODE_FUNC(NodeName, Class, Function)\
	auto* const NodeName = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);\
	NodeName->SetFromFunction(Class::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(Class, Function)));\

#define MAKE_DELEGATE_NODE(NodeName)\
	auto* const NodeName = CompilerContext.SpawnIntermediateNode<UK2Node_AddDelegate>(this, SourceGraph);\
	NodeName->SetFromProperty()

#define GET_PIN(NodeName, PinName)\
	UEdGraphPin* const NodeName##_##PinName = NodeName->FindPin(TEXT("PinName")));\
	
void UK2Node_BangoRunScript::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	ExpandSplitPins(CompilerContext, SourceGraph);

	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	check(SourceGraph && Schema);
	bool bIsErrorFree = true;

	UEdGraphPin* This_Script = FindPin(TEXT("Script"));

	// Make nodes
	MAKE_NODE(Node_ConstructObject, UK2Node_GenericCreateObject);
	MAKE_NODE(Node_IfThenElse, UK2Node_IfThenElse);
	MAKE_NODE(Node_CustomEvent, UK2Node_CustomEvent);
	MAKE_NODE(Node_AddDelegate, UK2Node_AddDelegate);
	MAKE_NODE(Node_CreateDelegate, UK2Node_CreateDelegate);
	MAKE_NODE(Node_Self, UK2Node_Self)
	MAKE_NODE_FUNC(Node_ExecuteScript, UBangoScriptObject, Execute_Internal);
	MAKE_NODE_FUNC(Node_IsValid, UKismetSystemLibrary, IsValid);

	// For non-dynamic nodes, generate pins and get pins
	Node_ConstructObject->AllocateDefaultPins();
	Node_IfThenElse->AllocateDefaultPins();
	Node_ExecuteScript->AllocateDefaultPins();
	Node_IsValid->AllocateDefaultPins();
	Node_CreateDelegate->AllocateDefaultPins();
	Node_Self->AllocateDefaultPins();
	
	UEdGraphPin* Pin_ConstructObject_Exec = Node_ConstructObject->GetExecPin();
	UEdGraphPin* Pin_ConstructObject_Then = Node_ConstructObject->GetThenPin();
	UEdGraphPin* Pin_ConstructObject_Result = Node_ConstructObject->GetResultPin();
	UEdGraphPin* Pin_ConstructObject_Class = Node_ConstructObject->FindPin(TEXT("Class"));
	
	UEdGraphPin* Pin_IfThenElse_Exec = Node_IfThenElse->GetExecPin();
	UEdGraphPin* Pin_IfThenElse_Condition = Node_IfThenElse->GetConditionPin();
	UEdGraphPin* Pin_IfThenElse_Then = Node_IfThenElse->GetThenPin();
	UEdGraphPin* Pin_IfThenElse_Else = Node_IfThenElse->GetElsePin();

	//UEdGraphPin* Pin_CustomEvent_OutputDelegate = Node_CustomEvent->FindPinChecked(TEXT("OutputDelegate"));
	
	UEdGraphPin* Pin_ExecuteScript_Exec = Node_ExecuteScript->GetExecPin();
	UEdGraphPin* Pin_ExecuteScript_Then = Node_ExecuteScript->GetThenPin();
	UEdGraphPin* Pin_ExecuteScript_Target = Node_ExecuteScript->FindPin(UEdGraphSchema_K2::PN_Self);
	UEdGraphPin* Pin_ExecuteScript_Result = Node_ExecuteScript->FindPin(UEdGraphSchema_K2::PN_ReturnValue);
	
	UEdGraphPin* Pin_IsValid_Object = Node_IsValid->FindPinChecked(TEXT("Object"));
	UEdGraphPin* Pin_IsValid_Result = Node_IsValid->GetReturnValuePin();

	UEdGraphPin* Pin_CreateDelegate_DelegateOut = Node_CreateDelegate->GetDelegateOutPin();
	UEdGraphPin* Pin_CreateDelegate_ObjectIn = Node_CreateDelegate->GetObjectInPin();

	UEdGraphPin* Pin_Self_Self = Node_Self->FindPinChecked(UEdGraphSchema_K2::PN_Self);
	
	// Get pins of our Run Script node
	UEdGraphPin* Pin_ThisNode_Exec = GetExecPin();
	UEdGraphPin* Pin_ThisNode_Then = GetThenPin();
	UEdGraphPin* Pin_ThisNode_Finish = FindPinChecked(UEdGraphSchema_K2::PN_Completed);
	UEdGraphPin* Pin_ThisNode_Handle = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);

	CompilerContext.MovePinLinksToIntermediate(*Pin_ThisNode_Handle, *Pin_ExecuteScript_Result);

	// Update the ConstructObject class pin; we need to generate pin collections before and after assigning the class so we can get the extra pins
	// TODO is there a different way I can just find ExposeOnSpawn pins?
	auto DefaultPins = TSet<UEdGraphPin*>(Node_ConstructObject->GetAllPins());

	Pin_ConstructObject_Class->DefaultObject = This_Script->DefaultObject;
	Node_ConstructObject->PinDefaultValueChanged(Pin_ConstructObject_Class);
	 
	auto ConstructScriptAllPins = TSet<UEdGraphPin*>(Node_ConstructObject->GetAllPins()); 
	
	TArray<UEdGraphPin*> Pins_ThisNode_ExposeOnSpawn;
	
	UClass* ObjectClass = Cast<UClass>(Pin_ConstructObject_Class->DefaultObject);
	
	// Build an array of all ExposeOnSpawn pins
	Pins_ThisNode_ExposeOnSpawn = ConstructScriptAllPins.Difference(DefaultPins).Array();

	if (ObjectClass)
	{
		FMulticastDelegateProperty* ScriptOnFinishDelegateProperty = CastField<FMulticastDelegateProperty>(ObjectClass->FindPropertyByName("OnFinishDelegate"));
		Node_AddDelegate->SetFromProperty(ScriptOnFinishDelegateProperty, false, ObjectClass);
	}
	
	Node_AddDelegate->AllocateDefaultPins();
	UEdGraphPin* Pin_AddDelegate_Exec = Node_AddDelegate->GetExecPin();
	UEdGraphPin* Pin_AddDelegate_Then = Node_AddDelegate->GetThenPin();
	UEdGraphPin* Pin_AddDelegate_Target = Node_AddDelegate->FindPin(UEdGraphSchema_K2::PN_Self);
	UEdGraphPin* Pin_AddDelegate_Delegate = Node_AddDelegate->GetDelegatePin();

	Node_CustomEvent->CustomFunctionName = *FString::Printf(TEXT("%s_%s"), TEXT("OnFinishedDelegate"), *CompilerContext.GetGuid(this));
	Node_CustomEvent->AllocateDefaultPins();
	UEdGraphPin* Pin_CustomEvent_Then = Node_CustomEvent->GetThenPin();
	
	bIsErrorFree &= Schema->TryCreateConnection(Pin_AddDelegate_Delegate, Pin_CreateDelegate_DelegateOut);
	//bIsErrorFree &= Schema->TryCreateConnection(Pin_AddDelegate_Delegate, Pin_CustomEvent_OutputDelegate);
	bIsErrorFree &= Schema->TryCreateConnection(Pin_Self_Self, Pin_CreateDelegate_ObjectIn);
	Node_CreateDelegate->SetFunction(Node_CustomEvent->CustomFunctionName);
	//bIsErrorFree &= Schema->TryCreateConnection(Pin_CustomEvent_Then, Pin_ThisNode_Finish);
	bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*Pin_ThisNode_Finish, *Pin_CustomEvent_Then).CanSafeConnect();
	
	// Hook up exec pins
	bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*Pin_ThisNode_Exec, *Pin_ConstructObject_Exec).CanSafeConnect();
	bIsErrorFree &= Schema->TryCreateConnection(Pin_ConstructObject_Then, Pin_IfThenElse_Exec);
	bIsErrorFree &= Schema->TryCreateConnection(Pin_IfThenElse_Then, Pin_AddDelegate_Exec);
	bIsErrorFree &= Schema->TryCreateConnection(Pin_AddDelegate_Then, Pin_ExecuteScript_Exec);
	bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*Pin_ThisNode_Then, *Pin_ExecuteScript_Then).CanSafeConnect();

	// Hook up data pins
	bIsErrorFree &= Schema->TryCreateConnection(Pin_ConstructObject_Result, Pin_IsValid_Object);
	bIsErrorFree &= Schema->TryCreateConnection(Pin_IsValid_Result, Pin_IfThenElse_Condition);
	bIsErrorFree &= Schema->TryCreateConnection(Pin_ConstructObject_Result, Pin_ExecuteScript_Target);
	bIsErrorFree &= Schema->TryCreateConnection(Pin_ConstructObject_Result, Pin_AddDelegate_Target);
	
	for (UEdGraphPin* Pin : Pins_ThisNode_ExposeOnSpawn)
	{
		// Dynamic ExposeOnSpawn pin
		UEdGraphPin* NewScriptExposeOnSpawnPin = Pin;
		UEdGraphPin* ThisExposeOnSpawnPin = FindPin(Pin->GetFName());

		if (ThisExposeOnSpawnPin)
		{
			if (!ThisExposeOnSpawnPin->DoesDefaultValueMatchAutogenerated())
			{
				NewScriptExposeOnSpawnPin->DefaultValue = ThisExposeOnSpawnPin->DefaultValue;
				Node_ConstructObject->PinDefaultValueChanged(NewScriptExposeOnSpawnPin);
			}
			else if (ThisExposeOnSpawnPin->HasAnyConnections())
			{
				CompilerContext.MovePinLinksToIntermediate(*ThisExposeOnSpawnPin, *NewScriptExposeOnSpawnPin);
				Node_ConstructObject->PinConnectionListChanged(NewScriptExposeOnSpawnPin);
			}	
		}
	}

	BreakAllNodeLinks();
}

#undef MAKE_NODE

UEdGraphPin* UK2Node_BangoRunScript::GetScriptPin(const TArray<UEdGraphPin*>* PinsToSearch) const
{
	if (PinsToSearch)
	{
		for (UEdGraphPin* TestPin : *PinsToSearch)
		{
			if (TestPin && TestPin->PinName == FName("Script"))
			{
				return TestPin;
				break;
			}
		}
	}
	else
	{
		if (!CachedScriptPin)
		{
			const_cast<UK2Node_BangoRunScript*>(this)->CachedScriptPin = FindPinChecked(FName("Script"));
		}

		return CachedScriptPin;	
	}

	checkNoEntry();
	return nullptr;
}

FText UK2Node_BangoRunScript::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return LOCTEXT("RunScript_NodeTitle", "Run Script");
}

FText UK2Node_BangoRunScript::GetTooltipText() const
{
    return LOCTEXT("RunScript_TooltipText", "Run a script");
}

void UK2Node_BangoRunScript::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    UClass* Action = GetClass();

    if (ActionRegistrar.IsOpenForRegistration(Action))
    {
        UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(Action);
        check(Spawner);

        ActionRegistrar.AddBlueprintAction(Action, Spawner);
    }
}

void UK2Node_BangoRunScript::AllocateDefaultPins()
{
    Super::AllocateDefaultPins();

    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);

	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

    UEdGraphPin* ScriptHandlePin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, FBangoScriptHandle::StaticStruct(), UEdGraphSchema_K2::PN_ReturnValue);
	ScriptHandlePin->PinFriendlyName = INVTEXT("Handle");
	
	UEdGraphPin* ScriptCompletePin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Completed);
	ScriptCompletePin->PinFriendlyName = INVTEXT("Finish");
	
	
    // TODO soft class???
    // TODO static pin name??? See FFormatTextNodeHelper
    
    CachedScriptPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Class, UBangoScriptObject::StaticClass(), FName("Script"));
}

UEdGraphPin* UK2Node_BangoRunScript::FindPropertyPin(const FName InPinName) const
{
    //const UEdGraphPin* ScriptPin = GetScriptPin();
    
    for (UEdGraphPin* Pin : Pins)
    {
        if( /*Pin != ScriptPin &&*/ Pin->Direction != EGPD_Output && Pin->PinName.ToString().Equals(InPinName.ToString(), ESearchCase::CaseSensitive) )
        {
            return Pin;
        }
    }

    return nullptr;
}

void UK2Node_BangoRunScript::UpdateScriptPins(UObject* InClassObject)
{
	UClass* InClass = Cast<UClass>(InClassObject);

	bool bChanged = false;

	PinNames.Reset();

	if (InClass)
	{
		for (TFieldIterator<FProperty> PropIt(InClass); PropIt; ++PropIt)
		{
			FProperty* Property = *PropIt;

			if (!Property->FindMetaData("ExposeOnSpawn"))
			{
				continue;
			}
        
			FFieldClass* PropertyClass = Property->GetClass();
        
			FName PinTypeName;
			bool bAddPin = false;

			// See EdGraphSchema_K2.h ~line 400 for all types
			TRY_PARSE_PIN_TYPE2(Bool, Boolean);
			TRY_PARSE_PIN_TYPE(Byte);
			TRY_PARSE_PIN_TYPE(Class)
			TRY_PARSE_PIN_TYPE(SoftClass)
			TRY_PARSE_PIN_TYPE(Int)
			TRY_PARSE_PIN_TYPE(Int64)
			TRY_PARSE_PIN_TYPE(Float)
			TRY_PARSE_PIN_TYPE(Double)
			// TRY_PARSE_PIN_TYPE(Real) // TODO Do I need this?
			TRY_PARSE_PIN_TYPE(Name)
			TRY_PARSE_PIN_TYPE(Delegate)
			TRY_PARSE_PIN_TYPE2(MulticastDelegate, MCDelegate)
			TRY_PARSE_PIN_TYPE(Object)
			TRY_PARSE_PIN_TYPE(Interface)
			TRY_PARSE_PIN_TYPE(SoftObject)
			TRY_PARSE_PIN_TYPE2(Str, String)
			TRY_PARSE_PIN_TYPE(Text)
			TRY_PARSE_PIN_TYPE(Struct)
			// TRY_PARSE_PIN_TYPE(Wildcard) // I should not need this
			TRY_PARSE_PIN_TYPE(Enum)
			TRY_PARSE_PIN_TYPE(FieldPath)
        
			if (bAddPin)
			{
				PinNames.Add(FName(Property->GetName()));

				bool bPinExists = !!FindPropertyPin(Property->GetFName());
				if (!bPinExists)
				{
					CreatePin(EGPD_Input, PinTypeName, FName(Property->GetName()));
					bChanged = true;
				}   
			}
		}
	}
    
	const UEdGraphPin* ScriptPin = GetScriptPin();
    
    for (auto It = Pins.CreateIterator(); It; ++It)
    {
        UEdGraphPin* CheckPin = *It;
        
        if (CheckPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec && CheckPin != ScriptPin && CheckPin->Direction == EGPD_Input)
        {
            const bool bIsValidArgPin = PinNames.ContainsByPredicate([&CheckPin](const FName& InPinName)
            {
                return InPinName == CheckPin->PinName;
            });

            if(!bIsValidArgPin)
            {
                CheckPin->MarkAsGarbage();
                It.RemoveCurrent();
            	bChanged = true;
            }
        }
    }

	if (bChanged)
	{
		GetGraph()->NotifyNodeChanged(this);
	}
}

#undef LOCTEXT_NAMESPACE
