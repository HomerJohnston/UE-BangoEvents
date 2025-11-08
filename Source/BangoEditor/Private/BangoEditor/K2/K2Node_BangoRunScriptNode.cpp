#include "K2Node_BangoRunScriptNode.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Bango/Core/BangoScriptObject.h"

void UK2Node_BangoRunScript::PinDefaultValueChanged(UEdGraphPin* Pin)
{
    const UEdGraphPin* ScriptPin = GetScriptPin();
    
    if (Pin == ScriptPin)
    {
        PinNames.Reset();
        
        UObject* ClassObject = ScriptPin->DefaultObject;
        UClass* Class = Cast<UClass>(ClassObject);

        for (TFieldIterator<FProperty> PropIt(Class); PropIt; ++PropIt)
        {
            FProperty* Property = *PropIt;

            FFieldClass* PropertyClass = Property->GetClass();
            
            FName PinTypeName;
            bool bAddPin = false;

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
                }   
            }
        }
        
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
                }
            }
        }
        
		GetGraph()->NotifyNodeChanged(this);
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

UEdGraphPin* UK2Node_BangoRunScript::GetScriptPin() const
{
    if (!CachedScriptPin)
    {
        const_cast<UK2Node_BangoRunScript*>(this)->CachedScriptPin = FindPinChecked(FName("Script"));
    }

    return CachedScriptPin;
}

FText UK2Node_BangoRunScript::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return GetClass()->GetDisplayNameText();
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
    
    // TODO soft class???
    // TODO static pin name??? See FFormatTextNodeHelper
    
    CachedScriptPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Class, UBangoScriptObject::StaticClass(), FName("Script"));
}

UEdGraphPin* UK2Node_BangoRunScript::FindPropertyPin(const FName InPinName) const
{
    const UEdGraphPin* ScriptPin = GetScriptPin();
    
    for (UEdGraphPin* Pin : Pins)
    {
        if( Pin != ScriptPin && Pin->Direction != EGPD_Output && Pin->PinName.ToString().Equals(InPinName.ToString(), ESearchCase::CaseSensitive) )
        {
            return Pin;
        }
    }

    return nullptr;
}