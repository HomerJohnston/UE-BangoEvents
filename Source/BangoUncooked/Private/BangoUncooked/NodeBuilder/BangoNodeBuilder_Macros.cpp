#include "BangoNodeBuilder_Macros.h"

#include "KismetCompiler.h"

BangoNodeBuilder::Builder::Builder(class FKismetCompilerContext& InContext, UEdGraph* InParentGraph, class UK2Node* InSourceNode, const UEdGraphSchema* InSchema, bool* InErrorBool, FVector2f Anchor)
{
	//UE_LOG(LogBango, Display, TEXT("Bango_NodeBuilder::Setup - bAwaitingSpawnFinish: %s"), bAwaitingSpawnFinish ? TEXT("TRUE" : TEXT("FALSE")));
	
	if (bAwaitingSpawnFinish)
	{
		// TODO breakpointing this or using a check crashes rider's debugger??? Maybe I should report it?
		UE_LOG(LogBango, Error, TEXT("Did you forget to call FinishSpawningAllNodes somewhere else?"))
	};

	_Compiler = &InContext;
	_ParentGraph = InParentGraph;
	_SourceNode = InSourceNode;
	_Schema = InSchema;
	_bErrorBool = InErrorBool;
	_GraphAnchor.X = Anchor.X; _GraphAnchor.Y = Anchor.Y;

	SpawnedNodes.Empty();
			
	bAwaitingSpawnFinish = true;
}

void BangoNodeBuilder::Builder::FinishDeferredNodes(bool bLogUnconnectedPins)
{
	//UE_LOG(LogBango, Display, TEXT("Bango_NodeBuilder::FinishSpawningAllNodes - bAwaitingSpawnFinish: %s"), bAwaitingSpawnFinish ? TEXT("TRUE" : TEXT("FALSE")));
	
	if (!bAwaitingSpawnFinish)
	{
		// TODO breakpointing this or using a check crashes rider's debugger??? Maybe I should report it?
		UE_LOG(LogBango, Error, TEXT("Did you forget to call FinishSpawningAllNodes somewhere else?"))
	};
			
	for (NodeWrapper_Base* Node : SpawnedNodes)
	{
		Node->DeferredConstruction();

		if (bLogUnconnectedPins)
		{
			for (UEdGraphPin* Pin : Node->BaseNode()->GetAllPins())
			{
				if (!Pin->HasAnyConnections())
				{
					UE_LOG(LogBango, Warning, TEXT("Unconnected pin --> Node: {%s} --> Pin: {%s}"), *Node->BaseNode()->GetDescriptiveCompiledName(), *Pin->GetName());
				}
			}
		}
	}

	bAwaitingSpawnFinish = false;
}

void BangoNodeBuilder::Builder::MoveExternalConnection(UEdGraphPin* From, UEdGraphPin* To)
{
	*_bErrorBool &= _Compiler->MovePinLinksToIntermediate(*From, *To).CanSafeConnect();
}

void BangoNodeBuilder::Builder::CopyExternalConnection(UEdGraphPin* From, UEdGraphPin* To)
{
	*_bErrorBool &= _Compiler->MovePinLinksToIntermediate(*From, *To).CanSafeConnect();
}

void BangoNodeBuilder::Builder::CreateConnection(UEdGraphPin* From, UEdGraphPin* To)
{
	*_bErrorBool &= _Schema->TryCreateConnection(From, To);
}

void BangoNodeBuilder::Builder::SetDefaultValue(UEdGraphPin* Pin, FString& Value)
{
	_Schema->TrySetDefaultValue(*Pin, Value);
}

void BangoNodeBuilder::Builder::SetDefaultObject(UEdGraphPin* Pin, UObject* Value)
{
	_Schema->TrySetDefaultObject(*Pin, Value);
}
