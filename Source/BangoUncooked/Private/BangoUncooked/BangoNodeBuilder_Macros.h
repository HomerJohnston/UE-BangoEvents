#pragma once

#include "EdGraph/EdGraph.h"

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
		CONCAT(IF_DEFERRED_, USE_DEFERRED)(bool UseDeferredConstruction() override { return true; })\
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
	
	virtual bool UseDeferredConstruction()
	{
		return false;
	}
		
	virtual void NormalConstruction()
	{
		if (UseDeferredConstruction() || bFinishedSpawning)
		{
			return;
		}

		Construct();
		bFinishedSpawning = true;
	}
		
	void DeferredConstruction()
	{
		if (!UseDeferredConstruction() || bFinishedSpawning)
		{
			return;
		}
		
		Construct();
		bFinishedSpawning = true;
	}	

	virtual void Construct() {};

	virtual UEdGraphNode* BaseNode() { return nullptr; }
};

namespace Bango_NodeBuilder
{
	struct Builder
	{
		class FKismetCompilerContext* _Compiler;
		class UEdGraph* _ParentGraph;
		class UK2Node* _SourceNode;
		FVector2f _GraphAnchor;

		const float _GraphAnchorScale = 300.0f; // Each major cell in hte grid is roughly 125 units; this lets me increment positions in units of '1' to space out my nodes

		const UEdGraphSchema* _Schema;
		bool* _bErrorBool;

		TArray<TSharedPtr<NodeWrapper_Base>> SpawnedNodes;
		bool bAwaitingSpawnFinish = false;
		
		Builder(class FKismetCompilerContext& InContext, UEdGraph* InParentGraph, class UK2Node* InSourceNode, const UEdGraphSchema* InSchema, bool* InErrorBool, FVector2f Anchor = FVector2f::ZeroVector)
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

		void FinishSpawningAllNodes(bool bLogUnconnectedPins = false)
		{
			checkf(bAwaitingSpawnFinish, TEXT("Did you forget to call Setup?"));
			
			for (TSharedPtr<NodeWrapper_Base> Node : SpawnedNodes)
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

		void MoveExternalConnection(UEdGraphPin* From, UEdGraphPin* To)
		{
			*_bErrorBool &= _Compiler->MovePinLinksToIntermediate(*From, *To).CanSafeConnect();
		}
		
		void CopyExternalConnection(UEdGraphPin* From, UEdGraphPin* To)
		{
			*_bErrorBool &= _Compiler->MovePinLinksToIntermediate(*From, *To).CanSafeConnect();
		}

		void CreateConnection(UEdGraphPin* From, UEdGraphPin* To)
		{
			*_bErrorBool &= _Schema->TryCreateConnection(From, To);
		}

		void SetDefaultValue(UEdGraphPin* Pin, FString& Value)
		{
			_Schema->TrySetDefaultValue(*Pin, Value);
		}
		
		void SetDefaultObject(UEdGraphPin* Pin, UObject* Value)
		{
			_Schema->TrySetDefaultObject(*Pin, Value);
		}

		template<typename TT>
		TSharedPtr<TT> MakeNode(float X, float Y)
		{
			TSharedPtr<TT> NewNodeWrapper = MakeShared<TT>(X, Y, _Compiler, _SourceNode, _ParentGraph);
			NewNodeWrapper->NormalConstruction();
			
			SpawnedNodes.Add(NewNodeWrapper);
			
			return NewNodeWrapper;
		}
		
		template<typename TT, typename T>
		TSharedPtr<TT> WrapExistingNode(T* InNode)
		{
			TSharedPtr<TT> NewNodeWrapper = MakeShared<TT>(InNode);
			NewNodeWrapper->NormalConstruction();

			SpawnedNodes.Add(NewNodeWrapper);
			
			return NewNodeWrapper;
		}
	};
		
	template<typename T>
	struct NodeWrapper : public NodeWrapper_Base
	{
	protected:
		T* _Node;

		bool bFromExisting = false;

	private:
		NodeWrapper()
		{
			checkNoEntry();
		}

	public:
		NodeWrapper(float X, float Y, FKismetCompilerContext* _Compiler, UK2Node* _SourceNode, UEdGraph* _ParentGraph)
		{
			_Node = _Compiler->SpawnIntermediateNode<T>(_SourceNode, _ParentGraph);
				
			//Node->SetNodePosX(_GraphAnchorScale *(_GraphAnchor.X + X));
			//Node->SetNodePosY(_GraphAnchorScale * (_GraphAnchor.Y + Y));
		}
			
		NodeWrapper(T* InNode)
		{
			bFromExisting = true;
			_Node = InNode;
		}


		void AllocateDefaultPins()
		{
			if (!bFromExisting)
			{
				_Node->AllocateDefaultPins();
			}
		}
		
		UEdGraphPin* FindPin(const char* PinName, EBangoPinRequired PinRequired = EBangoPinRequired::Required)
		{
			return FindPin(FName(PinName));
		}
		
		UEdGraphPin* FindPin(FName PinName, EBangoPinRequired PinRequired = EBangoPinRequired::Required)
		{
			UEdGraphPin* Pin = _Node->FindPin(PinName);

			if (PinRequired == EBangoPinRequired::Required)
			{
				check(Pin);
			}
				
			return Pin;
		}

		virtual UEdGraphNode* BaseNode() { return reinterpret_cast<UEdGraphNode*>(_Node); }

		T* Node() { return _Node; }
	};
}
