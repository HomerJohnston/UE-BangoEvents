#include "BangoUncooked/K2Nodes/K2Node_BangoFindActor.h"

#include "K2Node_DynamicCast.h"
#include "Bango/Components/BangoActorIDComponent.h"
#include "Bango/Subsystem/BangoActorIDSubsystem.h"
#include "Bango/Utility/BangoColor.h"
#include "Bango/Utility/BangoHelpers.h"
#include "BangoUncooked/NodeBuilder/BangoNodeBuilder.h"
#include "BangoUncooked/NodeBuilder/BangoNodeBuilder_Macros.h"

#define LOCTEXT_NAMESPACE "BangoEditor"

UK2Node_BangoFindActor::UK2Node_BangoFindActor()
{
	bShowNodeProperties = true;
}

bool UK2Node_BangoFindActor::ShouldDrawCompact() const
{
	return !TargetActor.IsNull();
}

FLinearColor UK2Node_BangoFindActor::GetNodeBodyTintColor() const
{
	return BangoColor::LightBlue;
}

FLinearColor UK2Node_BangoFindActor::GetNodeTitleColor() const
{
	return Super::GetNodeTitleColor();
}

FLinearColor UK2Node_BangoFindActor::GetNodeTitleTextColor() const
{
	return BangoColor::Orange;
}

void UK2Node_BangoFindActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.MemberProperty == StaticClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(ThisClass, CastTo)))
	{
		ReconstructNode();
	}
}

void UK2Node_BangoFindActor::AllocateDefaultPins()
{
	auto* BangoNamePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, FName("BangoName"));
	auto* BangoGuidPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FName("BangoGuid")); 
	auto* FoundActorPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object, FName("FoundActor"));
	
	// This pin is hidden when the node is set to a specific target actor
	BangoNamePin->bHidden = !TargetActor.IsNull(); 
	
	// Make it show an empty label // TODO is there a nicer way to do this?
	BangoNamePin->PinFriendlyName = LOCTEXT("BangoFindActorNode_BangoNamePinLabel", " ");
	
	// Never need to edit this pin, it's always null or automatically set
	BangoGuidPin->bHidden = true;
	
	if (IsValid(CastTo))
	{
		FoundActorPin->PinType.PinSubCategoryObject = CastTo;
	}
	else
	{
		FoundActorPin->PinType.PinSubCategoryObject = AActor::StaticClass();		
	}
	
	FoundActorPin->PinFriendlyName = LOCTEXT("BangoFindActorNode_FoundActorLabel", " ");
}

FText UK2Node_BangoFindActor::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (!TargetActor.IsNull())
	{
		if (TargetActor.IsValid())
		{
			return FText::FromString(TargetActor->GetActorLabel());
		}
		else
		{
			if (!CachedActorLabel.IsEmpty())
			{
				return FText::FromString(CachedActorLabel);
			}
			
			FString SubPath = TargetActor.ToSoftObjectPath().GetSubPathString();
			
			int32 LastDotIndex;
			if (SubPath.FindLastChar(TEXT('.'), LastDotIndex))
			{
				FString ActorName = SubPath.Mid(LastDotIndex + 1);
				
				return FText::Format(LOCTEXT("BangoFindActorNodeTitle_Unloaded", "{0}"), FText::FromString(ActorName));
			}
			
			checkNoEntry();
		}
	}
	
	return LOCTEXT("BangoFindActorNode_Title", "Find Actor");
}

void UK2Node_BangoFindActor::ExpandNode(class FKismetCompilerContext& Compiler, UEdGraph* SourceGraph)
{
	// -----------------
	// Error checking and fixup - if the node has a soft actor ptr assigned, we can check it. This works for manually dragged-in actor references only.
	// TODO I should lock renaming actor IDs behind a system so I can fix up all blueprints automatically?
	
	if (TargetActor.IsValid())
	{
		CachedActorLabel = TargetActor->GetActorLabel();

		UBangoActorIDComponent* IDComponent = Bango::GetActorIDComponent(TargetActor.Get());
		
		if (IDComponent)
		{
			CachedBangoName = IDComponent->GetBangoName().ToString();
		}
		else
		{
			Compiler.MessageLog.Error(*LOCTEXT("MissingActorIDComponent", "Actor is missing an ID component! @@").ToString(), this);
		}
	}

	Super::ExpandNode(Compiler, SourceGraph);

	const UEdGraphSchema_K2* Schema = Compiler.GetSchema();
	bool bIsErrorFree = true;

	namespace NB = BangoNodeBuilder;
	NB::Builder Builder(Compiler, SourceGraph, this, Schema, &bIsErrorFree, FVector2f(5, 5));
	
	// -----------------
	// Make nodes
	
	auto Node_This =					Builder.WrapExistingNode<NB::BangoFindActor>(this);
	auto Node_FindActorFunction	=		Builder.MakeNode<NB::CallFunction>(0, 1);
	auto Node_CastToType =				Builder.MakeNode<NB::DynamicCast_Pure>(1, 1);
	
	// -----------------
	// Post-setup

	FString UniqueID = *Compiler.GetGuid(this);
	
	if (!TargetActor.IsNull())
	{
		Node_FindActorFunction->SetFromFunction(UBangoActorIDBlueprintFunctionLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoActorIDBlueprintFunctionLibrary, K2_GetActorByGuid)));
	}
	else
	{
		Node_FindActorFunction->SetFromFunction(UBangoActorIDBlueprintFunctionLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UBangoActorIDBlueprintFunctionLibrary, K2_GetActorByName)));
	}
	
	if (IsValid(CastTo))
	{
		Node_CastToType->TargetType = CastTo;
	}
	
	Builder.FinishDeferredNodes();
	
	// -----------------
	// Make connections
	
	if (!TargetActor.IsNull())
	{
		FString GuidAsString = TargetBangoGuid.ToString(EGuidFormats::Digits);
		Builder.SetDefaultValue(Node_FindActorFunction.FindPin("Guid"), GuidAsString);
	}
	else if (Node_This.BangoName->HasAnyConnections())
	{
		Builder.CopyExternalConnection(Node_This.BangoName, Node_FindActorFunction.FindPin("Name"));
	}
	else
	{
		Builder.SetDefaultValue(Node_FindActorFunction.FindPin("Name"), Node_This.BangoName->DefaultValue);
	}
	
	if (IsValid(CastTo))
	{
		Builder.CreateConnection(Node_FindActorFunction->GetReturnValuePin(), Node_CastToType.ObjectToCast);
		Builder.CopyExternalConnection(Node_This.FoundActor, Node_CastToType.CastedObject);
	}
	else
	{
		Builder.CopyExternalConnection(Node_This.FoundActor, Node_FindActorFunction->GetReturnValuePin());
	}
	
	// Done!
	if (!bIsErrorFree)
	{
		Compiler.MessageLog.Error(*LOCTEXT("InternalConnectionError", "Internal connection error. @@").ToString(), this);
	}
	
	// Disconnect ThisNode from the graph
	BreakAllNodeLinks();
}

void UK2Node_BangoFindActor::SetActor(AActor* Actor)
{
	UBangoActorIDComponent* IDComponent = Bango::GetActorIDComponent(Actor);
	
	if (IDComponent)
	{
		TargetActor = Actor;
		TargetBangoGuid = IDComponent->GetBangoGuid();
		TargetName = NAME_None;
		CastTo = Actor->GetClass();
	}
}

void UK2Node_BangoFindActor::PostCDOCompiled(const FPostCDOCompiledContext& Context)
{
	
	
	Super::PostCDOCompiled(Context);
}

AActor* UK2Node_BangoFindActor::GetReferencedLevelActor() const
{
	if (TargetActor.IsPending())
	{
		UE_LOG(LogBango, Warning, TEXT("Actor's level is unloaded; can't jump to it!"));
	}
	
	else if (TargetActor.IsValid())
	{
		return TargetActor.Get();
	}
	
	return nullptr;
}

void UK2Node_BangoFindActor::ReconstructNode()
{
	Super::ReconstructNode();
}

void UK2Node_BangoFindActor::PreloadRequiredAssets()
{
}

void UK2Node_BangoFindActor::PostReconstructNode()
{
	Super::PostReconstructNode();
}

void UK2Node_BangoFindActor::ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);
}

#undef LOCTEXT_NAMESPACE
