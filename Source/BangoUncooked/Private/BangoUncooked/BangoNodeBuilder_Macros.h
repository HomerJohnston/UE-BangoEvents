#pragma once

// ==============================================

#define MAKE_NODE_TYPE(NodeType, Name, ...)\
	struct NodeType : public NodeWrapper<Name>\
	{\
		using NodeWrapper::NodeWrapper;\
		/*Default pins*/\
		UEdGraphPin __VA_ARGS__;\
		\
		void Construct() override;\
	}\
	
// ==============================================

#define MAKE_NODE(Name, Type, X, Y, ...)\
	Bango_BuildNode::Type Node_##Name(X, Y, ##__VA_ARGS__);\
	Node_##Name.Construct_Internal()

// ==============================================

#define EXTERNAL_CONNECTION_MOVE(FromPin, ToPin)\
bIsErrorFree &= Compiler.MovePinLinksToIntermediate(*FromPin, *ToPin).CanSafeConnect()
	
#define EXTERNAL_CONNECTION_COPY(FromPin, ToPin)\
bIsErrorFree &= Compiler.CopyPinLinksToIntermediate(*FromPin, *ToPin).CanSafeConnect()

#define CREATE_CONNECTION(FromPin, ToPin)\
bIsErrorFree &= Schema->TryCreateConnection(FromPin, ToPin);

// ==============================================

enum class EBangoDeferConstruction : uint8
{
	Normal = 0,
	DeferredConstruction = 1,
	Finished = 2
};

namespace Bango_BuildNode
{
	static class FKismetCompilerContext* _Compiler;
	static class UEdGraph* _ParentGraph;
	static class UK2Node* _SourceNode;
	static FVector2f _GraphAnchor(0, 0);

	static const UEdGraphSchema* _Schema;
	static bool* _bErrorBool;
	
	static void Setup(class FKismetCompilerContext& InContext, class UEdGraph* InParentGraph, class UK2Node* InSourceNode, const UEdGraphSchema* InSchema, bool* InErrorBool, FVector2f Anchor = FVector2f::ZeroVector)
	{
		_Compiler = &InContext; _ParentGraph = InParentGraph; _SourceNode = InSourceNode;
		_Schema = InSchema;
		_bErrorBool = InErrorBool;
		_GraphAnchor.X = Anchor.X; _GraphAnchor.Y = Anchor.Y;
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
	
	template<typename T>
	struct NodeWrapper
	{
	protected:
		T* Node;
		EBangoDeferConstruction DeferredConstruction = EBangoDeferConstruction::Normal;

	public:
		T* operator->()
		{
			return Node;
		}

	private:
		NodeWrapper()
		{
			Node = _Compiler->SpawnIntermediateNode<T>(_SourceNode, _ParentGraph);
		}

	public:
		NodeWrapper(float X, float Y, EBangoDeferConstruction InDeferredConstruction = EBangoDeferConstruction::Normal) : NodeWrapper()
		{
			Node->SetNodePosX(_GraphAnchor.X + X);
			Node->SetNodePosY(_GraphAnchor.Y + Y);
			DeferredConstruction = InDeferredConstruction;
		}
		
		NodeWrapper(T* InNode)
		{
			Node = InNode;
		}

		virtual void Construct_Internal()
		{
			if (DeferredConstruction == EBangoDeferConstruction::Normal)
			{
				Construct();
			}
		}
		
		virtual void Construct() = 0;
			
		void FinishConstruction()
		{
			Construct();
		}
	
		UEdGraphPin* FindPin(const char* PinName)
		{
			UEdGraphPin* Pin = Node->FindPin(FName(PinName));
			check(Pin);
			return Pin;
		}
	};

	template<typename T>
	T MakeNode(float X, float Y, EBangoDeferConstruction DeferConstruction = EBangoDeferConstruction::Normal)
	{
		NodeWrapper<T> NewNodeWrapper(X, Y, DeferConstruction);
		NewNodeWrapper.Construct_Internal();
		return NewNodeWrapper;
	}

	template<typename T, typename Y>
	T MakeExistingNode(Y* InNode)
	{
		NodeWrapper<T> NewNodeWrapper(InNode);
		NewNodeWrapper.Construct_Internal();
		return NewNodeWrapper;
	}
}
