#pragma once

// ==============================================

#define NORMAL_CONSTRUCTION 0
#define DEFERRED_CONSTRUCTION 1

#define IF_DEFERRED_0(code)
#define IF_DEFERRED_1(code) code

#define CONCAT(A, B) A##B
#define EXPAND(X) X

#define STRIP_AST()

#define DECL_PIN(Name) UEdGraphPin* Name;
#define ARR_PIN(Name) Name,
	
#define FOR_EACH_1(M, a) M(a)
#define FOR_EACH_2(M, a, b) M(a) M(b)
#define FOR_EACH_3(M, a, b, c) M(a) M(b) M(c)
#define FOR_EACH_4(M, a, b, c, d) M(a) M(b) M(c) M(d)
#define FOR_EACH_5(M, a, b, c, d, e) M(a) M(b) M(c) M(d) M(e)
#define FOR_EACH_6(M, a, b, c, d, e, f) M(a) M(b) M(c) M(d) M(e) M(f)
#define FOR_EACH_7(M, a, b, c, d, e, f, g) M(a) M(b) M(c) M(d) M(e) M(f) M(g)
#define FOR_EACH_8(M, a, b, c, d, e, f, g, h) M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h)
#define FOR_EACH_9(M, a, b, c, d, e, f, g, h, i) M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i)
#define FOR_EACH_10(M, a, b, c, d, e, f, g, h, i, j) M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i) M(j)

#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, NAME, ...) NAME
#define FOR_EACH(M, ...) GET_MACRO(__VA_ARGS__, FOR_EACH_10, FOR_EACH_9, FOR_EACH_8, FOR_EACH_7, FOR_EACH_6, FOR_EACH_5, FOR_EACH_4, FOR_EACH_3, FOR_EACH_2, FOR_EACH_1)(M, __VA_ARGS__)

#define MAKE_NODE_TYPE(NodeType, Name, USE_DEFERRED, ...)\
	struct NodeType : public NodeWrapper<Name>\
	{\
		using NodeWrapper::NodeWrapper;\
		\
		virtual ~NodeType() {};\
		\
		CONCAT(IF_DEFERRED_, USE_DEFERRED)(bool DeferredConstruction() override { return true; })\
		\
		/*Declare pins*/\
		FOR_EACH(DECL_PIN, __VA_ARGS__)\
		\
		void Construct() override;\
	}\
	
#include "Bango/Utility/BangoLog.h"
// ==============================================

enum class EBangoPinRequired : uint8
{
	Required,
	Optional,
};

struct NodeWrapper_Base
{
	bool bFinishedSpawning = false;
	
	virtual bool DeferredConstruction()
	{
		return false;
	}
		
	virtual void Construct_Internal()
	{
		if (!DeferredConstruction() && !bFinishedSpawning)
		{
			Construct();
			bFinishedSpawning = true;
		}
	}
		
	virtual void Construct() {};
			
	void FinishConstruction()
	{
		if (DeferredConstruction() && !bFinishedSpawning)
		{
			Construct();
			bFinishedSpawning = true;
		}
	}

	virtual UEdGraphNode* GetNode() { return nullptr; }
};

namespace Bango_NodeBuilder
{
	static class FKismetCompilerContext* _Compiler;
	static class UEdGraph* _ParentGraph;
	static class UK2Node* _SourceNode;
	static FVector2f _GraphAnchor(0, 0);

	static const float _GraphAnchorScale = 300.0f; // Each major cell in hte grid is roughly 125 units; this lets me increment positions in units of '1' to space out my nodes

	static const UEdGraphSchema* _Schema;
	static bool* _bErrorBool;

	static TArray<NodeWrapper_Base*> SpawnedNodes;
	bool bAwaitingSpawnFinish = false;
	
	static void Setup(class FKismetCompilerContext& InContext, class UEdGraph* InParentGraph, class UK2Node* InSourceNode, const UEdGraphSchema* InSchema, bool* InErrorBool, FVector2f Anchor = FVector2f::ZeroVector)
	{
		checkf(!bAwaitingSpawnFinish, TEXT("Did you forget to call FinishSpawningAllNodes somewhere else?"));

		_Compiler = &InContext;
		_ParentGraph = InParentGraph;
		_SourceNode = InSourceNode;
		_Schema = InSchema;
		_bErrorBool = InErrorBool;
		_GraphAnchor.X = Anchor.X; _GraphAnchor.Y = Anchor.Y;

		SpawnedNodes.Empty();
		
		bAwaitingSpawnFinish = true;
	}

	static void FinishSpawningAllNodes(bool bLogUnconnectedPins = false)
	{
		checkf(bAwaitingSpawnFinish, TEXT("Did you forget to call Setup?"));
		
		for (NodeWrapper_Base* Node : SpawnedNodes)
		{
			Node->FinishConstruction();

			if (bLogUnconnectedPins)
			{
				for (UEdGraphPin* Pin : Node->GetNode()->GetAllPins())
				{
					if (!Pin->HasAnyConnections())
					{
						UE_LOG(LogBango, Warning, TEXT("Unconnected pin --> Node: {%s} --> Pin: {%s}"), *Node->GetNode()->GetDescriptiveCompiledName(), *Pin->GetName());
					}
				}
			}
		}

		bAwaitingSpawnFinish = false;
	}

	static void MoveExternalConnection(UEdGraphPin* From, UEdGraphPin* To)
	{
		*_bErrorBool &= _Compiler->MovePinLinksToIntermediate(*From, *To).CanSafeConnect();
	}
	
	static void CopyExternalConnection(UEdGraphPin* From, UEdGraphPin* To)
	{
		*_bErrorBool &= _Compiler->MovePinLinksToIntermediate(*From, *To).CanSafeConnect();
	}

	static void CreateConnection(UEdGraphPin* From, UEdGraphPin* To)
	{
		*_bErrorBool &= _Schema->TryCreateConnection(From, To);
	}

	static void SetDefaultValue(UEdGraphPin* Pin, FString& Value)
	{
		_Schema->TrySetDefaultValue(*Pin, Value);
	}
	
	static void SetDefaultObject(UEdGraphPin* Pin, UObject* Value)
	{
		_Schema->TrySetDefaultObject(*Pin, Value);
	}
	
	template<typename T>
	struct NodeWrapper : public NodeWrapper_Base
	{
	protected:
		T* Node;

		bool bFromExisting = false;

	public:
		T* operator->()
		{
			return Node;
		}

	private:
		NodeWrapper()
		{
			checkNoEntry();
			//Node = _Compiler->SpawnIntermediateNode<T>(_SourceNode, _ParentGraph);
		}

	public:
		NodeWrapper(float X, float Y)
		{
			Node = _Compiler->SpawnIntermediateNode<T>(_SourceNode, _ParentGraph);
			Node->SetNodePosX(_GraphAnchorScale *(_GraphAnchor.X + X));
			Node->SetNodePosY(_GraphAnchorScale * (_GraphAnchor.Y + Y));
		}
		
		NodeWrapper(T* InNode)
		{
			bFromExisting = true;
			Node = InNode;
		}


		void AllocatePins()
		{
			if (!bFromExisting)
			{
				Node->AllocateDefaultPins();
			}
		}
	
		UEdGraphPin* FindPin(const char* PinName, EBangoPinRequired PinRequired = EBangoPinRequired::Required)
		{
			UEdGraphPin* Pin = Node->FindPin(FName(PinName));

			if (PinRequired == EBangoPinRequired::Required)
			{
				check(Pin);
			}
			
			return Pin;
		}

		UEdGraphNode* GetNode() override { return reinterpret_cast<UEdGraphNode*>(Node); }
	};
	
	template<typename TT>
	TT MakeNode(float X, float Y)
	{
		TT NewNodeWrapper(X, Y);
		NewNodeWrapper.Construct_Internal();
		
		SpawnedNodes.Add(&NewNodeWrapper);
		
		return NewNodeWrapper;
	}
	
	template<typename TT, typename T>
	TT WrapExistingNode(T* InNode)
	{
		TT NewNodeWrapper(InNode);
		NewNodeWrapper.Construct_Internal();

		SpawnedNodes.Add(&NewNodeWrapper);
		
		return NewNodeWrapper;
	}
}
