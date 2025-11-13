#pragma once

#define DECLARE_BASE(NodeName, NodeType)\
private:\
/*NodeName();*/\
public:\
/*NodeName(float X, float Y);*/\
NodeType* Node\


// ==============================================

#define MAKE_NODE_TYPE(NodeType, Name, ...)\
	struct NodeType : public NodeBase<Name>\
	{\
		using NodeBase::NodeBase;\
		/*Default pins*/\
		UEdGraphPin __VA_ARGS__;\
		\
		void Construct() override;\
	}\
	
// ==============================================

#define MAKE_NODE(Type, Name, X, Y, ...)\
	BangoK2NodeBuilder::Type Node_##Name(X, Y, ##__VA_ARGS__);\
	Node_##Name.Construct_Internal()

// ==============================================

#define NODE_BUILDER_BOILERPLATE\
	static class FKismetCompilerContext* Context;\
	static class UEdGraph* ParentGraph;\
	static class UK2Node* SourceNode;\
	\
	static void Setup(class FKismetCompilerContext& InContext, class UEdGraph* InParentGraph, class UK2Node* InSourceNode)\
	{\
		Context = &InContext;\
		ParentGraph = InParentGraph;\
		SourceNode = InSourceNode;\
	}\

enum class EBangoDeferConstruction : uint8
{
	Normal = 0,
	Deferred = 1,
	Finished = 2
};

namespace BangoK2NodeBuilder
{
	NODE_BUILDER_BOILERPLATE
	
	template<typename T>
	struct NodeBase
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
		NodeBase()
		{
			Node = Context->SpawnIntermediateNode<T>(SourceNode, ParentGraph);
		}

	public:
		NodeBase(float X, float Y, EBangoDeferConstruction InDeferredConstruction = EBangoDeferConstruction::Normal) : NodeBase()
		{
			Node->SetNodePosX(X);
			Node->SetNodePosY(Y);
			DeferredConstruction = InDeferredConstruction;
		}

		virtual void Construct_Internal()
		{
			if (DeferredConstruction == EBangoDeferConstruction::Normal)
			{
				Construct();
			}
		}
		
		virtual void Construct() = 0;
			
		void FinishDeferredConstruction()
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
}
