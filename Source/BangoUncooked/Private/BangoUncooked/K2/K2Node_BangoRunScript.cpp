#include "BangoUncooked/K2/K2Node_BangoRunScript.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_AddDelegate.h"
#include "K2Node_AssignDelegate.h"
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
#include "BangoUncooked/BangoNodeBuilder.h"
#include "BangoUncooked/BangoNodeBuilder_Macros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/PropertyIterator.h"

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
	bIsLatent = true;
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

void UK2Node_BangoRunScript::ExpandNode(class FKismetCompilerContext& Compiler, UEdGraph* SourceGraph)
{
	Super::ExpandNode(Compiler, SourceGraph);




	{
		UClass* ObjectClass = UBangoScriptObject::StaticClass();// Cast<UClass>(Node_CreateScriptObject->ObjectClass->DefaultObject);
	
		if (ObjectClass)
		{
			FProperty* OnFinishDelegate = ObjectClass->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UBangoScriptObject, OnFinishDelegate));
			FMulticastDelegateProperty* ScriptOnFinishDelegateProperty = CastField<FMulticastDelegateProperty>(ObjectClass->FindPropertyByName("OnFinishDelegate"));
		
			UK2Node_AddDelegate* Node_AddDelegate = Compiler.SpawnIntermediateNode<UK2Node_AssignDelegate>(this, SourceGraph);
			Node_AddDelegate->AllocateDefaultPins();
			Node_AddDelegate->SetFromProperty(ScriptOnFinishDelegateProperty, false, ObjectClass);
			Node_AddDelegate->AllocateDefaultPins();
		}
	}


	
	const UEdGraphSchema_K2* Schema = Compiler.GetSchema();
	bool bIsErrorFree = true;

	//Bango_NodeBuilder::Setup(Compiler, SourceGraph, this, Schema, &bIsErrorFree, FVector2f(0, 1));
	Bango_NodeBuilder::Builder Builder(Compiler, SourceGraph, this, Schema, &bIsErrorFree, FVector2f(0, 1));
	
	// -----------------
	// Make nodes
	
	using namespace Bango_NodeBuilder;
	auto Node_This =					Builder.WrapExistingNode<BangoRunScript>(this);
	auto Node_Self =					Builder.MakeNode<SelfReference>(0, 2);
	auto Node_CreateScriptObject =		Builder.MakeNode<CreateObject>(1, 1);
	auto Node_IsScriptValid =			Builder.MakeNode<IsValidPure>(2, 0);
	auto Node_IsValidBranch =			Builder.MakeNode<Branch>(3, 0);
	auto Node_AddDelegate =				Builder.MakeNode<AddDelegate>(4, 0);
	auto Node_ExecuteScript =			Builder.MakeNode<BangoExecuteScript_Internal>(5, 1);
	auto Node_ScriptCompletedEvent =	Builder.MakeNode<CustomEvent>(6, 1);
	auto Node_CreateDelegate =			Builder.MakeNode<CreateDelegate>(4, 1);

	// -----------------
	// Post-setup

	// Update the ConstructObject class pin; we need to generate pin collections before and after assigning the class so we can get the extra pins
	if (UClass* ScriptClass = Cast<UClass>(Node_This->Script->DefaultObject))
	{
		// There is a script set, we need to generate dynamic pins. Set the script, this will cause the intermediate script object creator to generate pins.
		Builder.SetDefaultObject(Node_CreateScriptObject->ObjectClass, Node_This->Script->DefaultObject);

		// We'll try to guess what pins we have by finding ExposeOnSpawn properties. This should be accurate? TODO investigate this more?
		TArray<FName> ExposeOnSpawnProperties;
		
		for (TFieldIterator<FProperty> It(ScriptClass); It; ++It)
		{
			FProperty* Property = *It;

			if (Property->HasAllPropertyFlags(CPF_ExposeOnSpawn))
			{
				ExposeOnSpawnProperties.Add(Property->GetFName());
			}
		}
		
		for (FName PropertyName : ExposeOnSpawnProperties)
		{
			UEdGraphPin* ThisExposeOnSpawnPin = FindPin(PropertyName);
			UEdGraphPin* ConstructScriptExposeOnSpawnPin = Node_CreateScriptObject->FindPin(PropertyName);

			if (ThisExposeOnSpawnPin && ConstructScriptExposeOnSpawnPin)
			{
				// If the dynamic pin has a hardcoded value set directly on it...
				if (!ThisExposeOnSpawnPin->DoesDefaultValueMatchAutogenerated())
				{
					Builder.SetDefaultValue(ConstructScriptExposeOnSpawnPin, ThisExposeOnSpawnPin->DefaultValue);
				}
				// If the dynamic pin has a connection...
				else if (ThisExposeOnSpawnPin->HasAnyConnections())
				{
					Builder.MoveExternalConnection(ThisExposeOnSpawnPin, ConstructScriptExposeOnSpawnPin);
					//Node_ConstructScript->PinConnectionListChanged(ConstructScriptExposeOnSpawnPin); // TODO do I need this?
				}
			}
		}
	}

	UClass* ObjectClass = Cast<UClass>(Node_CreateScriptObject->ObjectClass->DefaultObject);
	
	if (ObjectClass)
	{
		FProperty* OnFinishDelegate = ObjectClass->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UBangoScriptObject, OnFinishDelegate));
		Node_AddDelegate->Node()->SetFromProperty(OnFinishDelegate, false, ObjectClass);
	}
	
	Node_ScriptCompletedEvent->Node()->CustomFunctionName = *FString::Printf(TEXT("%s_%s"), TEXT("OnFinishedDelegate"), *Compiler.GetGuid(this));

	Builder.FinishSpawningAllNodes(true);

	// -----------------
	// Make connections

	if (ObjectClass)
	{
		Builder.CreateConnection(Node_AddDelegate->Delegate, Node_CreateDelegate->DelegateOut);
	}
	
	Builder.CreateConnection(Node_Self->Self, Node_CreateDelegate->ObjectIn);
	Node_CreateDelegate->Node()->SetFunction(Node_ScriptCompletedEvent->Node()->CustomFunctionName);
	Builder.MoveExternalConnection(Node_This->Completed, Node_ScriptCompletedEvent->Then);
	
	// Hook up exec pins
	Builder.MoveExternalConnection(Node_This->Exec, Node_CreateScriptObject->Exec);
	Builder.CreateConnection(Node_CreateScriptObject->Then, Node_IsValidBranch->Exec);
	Builder.CreateConnection(Node_IsValidBranch->Then, Node_AddDelegate->Exec);
	Builder.CreateConnection(Node_AddDelegate->Then, Node_ExecuteScript->Exec);
	Builder.MoveExternalConnection(Node_This->Then, Node_ExecuteScript->Then);

	// Hook up data pins
	Builder.CreateConnection(Node_CreateScriptObject->CreatedObject, Node_IsScriptValid->Object);
	Builder.CreateConnection(Node_IsScriptValid->Result, Node_IsValidBranch->Condition);
	Builder.CreateConnection(Node_CreateScriptObject->CreatedObject, Node_ExecuteScript->Target);
	Builder.CreateConnection(Node_CreateScriptObject->CreatedObject, Node_AddDelegate->Target);

	// Final output
	Builder.MoveExternalConnection(Node_This->Handle, Node_ExecuteScript->Result);

	if (!bIsErrorFree)
	{
		Compiler.MessageLog.Error(*LOCTEXT("InternalConnectionError", "K2Node_LoadAsset: Internal connection error. @@").ToString(), this);
	}
	
	// All done!
	BreakAllNodeLinks();
}

#undef MAKE_BASIC_NODE_OLD

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
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);

	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

    UEdGraphPin* ScriptHandlePin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, FBangoScriptHandle::StaticStruct(), UEdGraphSchema_K2::PN_ReturnValue);
	ScriptHandlePin->PinFriendlyName = INVTEXT("Handle");
	
	UEdGraphPin* ScriptCompletePin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Completed);
	ScriptCompletePin->PinFriendlyName = INVTEXT("Finish");
	
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
