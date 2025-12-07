#include "BangoScriptValidator.h"

#include "K2Node_Event.h"
#include "Bango/Core/BangoScriptObject.h"
#include "BangoUncooked/K2Nodes/K2Node_BangoFinishScript.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

void UBangoScriptValidator::Initialize(FSubsystemCollectionBase& Collection)
{
	UBangoScriptInstance::OnScriptRequestValidation.BindStatic(&ThisClass::IsScriptDataValid);
}

EDataValidationResult UBangoScriptValidator::IsScriptDataValid(class FDataValidationContext& Context, const UBangoScriptInstance* ScriptObject)
{
	UBlueprint* Blueprint = UBlueprint::GetBlueprintFromClass(ScriptObject->GetClass());
	
	if (!Blueprint)
	{
		return EDataValidationResult::NotValidated;
	}
	
	const auto& Ubergraphs = Blueprint->UbergraphPages;
	
	//UE_LOG(LogTemp, Display, TEXT("----------"));
	
	int32 CustomEventBinds = 0;
	int32 FinishNodesNum = 0;
	int32 UnconnectedFinishNodesNum = 0;
	int32 UbergraphsNum = 0;
	bool bFoundUberGraphWithoutFinishNode = false;
	
	for (const auto& Ubergraph : Ubergraphs)
	{
		++UbergraphsNum;
		
		//UE_LOG(LogTemp, Display, TEXT("Ubergraph: %s"), *Ubergraph.GetName())
		const auto& Nodes = Ubergraph->Nodes;
		
		bool bUberGraphHasFinishNode = false;
		
		for (const TObjectPtr<class UEdGraphNode>& Node : Nodes)
		{
			//UE_LOG(LogTemp, Display, TEXT("    Node: %s"), *Node.GetName());
			
			if (UK2Node_Event* EventNode = Cast<UK2Node_Event>(Node))
			{
				if (EventNode->GetDelegatePin()->HasAnyConnections())
				{
					++CustomEventBinds;
				}
			}
			
			if (UK2Node_BangoFinishScript* FinishNode = Cast<UK2Node_BangoFinishScript>(Node))
			{
				++FinishNodesNum;
				
				if (!FinishNode->GetExecPin()->HasAnyConnections())
				{
					++UnconnectedFinishNodesNum;
				}
				
				bUberGraphHasFinishNode = true;
			}
		}
		
		bFoundUberGraphWithoutFinishNode |= !bUberGraphHasFinishNode;
	}
	
	if (ScriptObject->bPreventAutoDestroy)
	{
		if (!FinishNodesNum)
		{
			Context.AddWarning(LOCTEXT("BangoScriptValidator_Warning1", "Script is set to prevent automatic destruction, but contains no Finish nodes; this script object will never be destroyed!"));
		}
		else if (UnconnectedFinishNodesNum > 0)
		{
			Context.AddWarning(LOCTEXT("BangoScriptValidator_Warning2", "Script is set to prevent automatic destruction, but contains unconnected finish nodes; if a Finish node is not called, this script object will never be destroyed!"));
		}
	}
	else
	{
		if (CustomEventBinds)
		{
			Context.AddWarning(LOCTEXT("BangoScriptValidator_Warning3", "Script contains custom events, but is not set to be kept alive while idle; the running script may be destroyed before the event is called if it finishes running all nodes!"));
		}
		
		if (UbergraphsNum > 1)
		{
			Context.AddWarning(LOCTEXT("BangoScriptValidator_Warning4", "Script contains multiple event graphs, but is not set to be kept alive while idle; the running script may be destroyed before other events are executed if it finishes running all nodes!"));
			
			if (bFoundUberGraphWithoutFinishNode)
			{
				Context.AddWarning(LOCTEXT("BangoScriptValidator_Warning5", "Script is set to be kept alive while idle but not all event graphs have Finish nodes; the running script may never be destroyed if a Finish node is not called!"));
			}
		}
	}
	
	return EDataValidationResult::Valid;
}

#undef LOCTEXT_NAMESPACE