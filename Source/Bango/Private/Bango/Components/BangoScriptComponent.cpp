#include "Bango/Components/BangoScriptComponent.h"

#include "Bango/Core/BangoBlueprintFunctionLibrary.h"
#include "Bango/Core/BangoScript.h"
//#include "Bango/Editor/BangoScriptHelperSubsystem.h"
#include "Bango/Subsystem/BangoScriptSubsystem.h"
#include "Bango/Utility/BangoHelpers.h"
#include "Bango/Utility/BangoLog.h"
#include "Exporters/Exporter.h"
#include "UObject/ObjectSaveContext.h"
#include "UObject/PropertyAccessUtil.h"

UBangoScriptComponent::UBangoScriptComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBangoScriptComponent::PrintState(FString Msg) const
{
	bool bIsTemplate = IsTemplate();
	bool bIsValid = IsValid(this);
	bool bMirroredGarbage = HasAllFlags(RF_MirroredGarbage);
	bool bGarbage = HasAnyInternalFlags(EInternalObjectFlags::Garbage);
	bool bUnreachable = HasAnyInternalFlags(EInternalObjectFlags::Unreachable);
	bool bPendingConstruction = HasAnyInternalFlags(EInternalObjectFlags::PendingConstruction);
	
	uint32 Flags = (uint32)GetOwner()->GetFlags();
	uint32 IntFlags = (uint32)GetOwner()->GetInternalFlags();
	
	FString BitString1;
	FString BitString2;
	BitString1.Reserve(32);
	BitString2.Reserve(32);

	while (Msg.Len() < 32)
	{
		Msg += " ";
	}
	
	for (int32 i = 31; i >= 0; --i)
	{
		BitString1.AppendChar(((Flags >> i) & 1) ? TEXT('1') : TEXT('0'));
		BitString2.AppendChar(((IntFlags >> i) & 1) ? TEXT('1') : TEXT('0'));
	}
	
	UE_LOG(LogBango, Display, TEXT("%s"), *FString::Format(TEXT("{0}: {1} | {2}"), { Msg, *BitString1, *BitString2 } ));
}

void UBangoScriptComponent::OnRegister()
{
	Super::OnRegister();
	
	
}

void UBangoScriptComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (bRunOnBeginPlay)
	{	
		Run();
	}
}

void UBangoScriptComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void UBangoScriptComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	
	PrintState("OnComponentCreated");
	
	// With RF_LoadCompleted this is a default actor component.
	
	// Because UE creates and destroys components a thousand times a second, we will not use this.
	// We will instead create the script using the details customization on the script container. This is probably more reliable anyway.  
	if (HasAllFlags(RF_LoadCompleted))
	{
		/*
		if (this->GetWorld() && this->ComponentIsInPersistentLevel(false))
		{
			auto Lambda = FTimerDelegate::CreateLambda([this] ()
			{
				FBangoEditorDelegates::OnScriptContainerCreated.Broadcast(this, &Script);
			});
			this->GetWorld()->GetTimerManager().SetTimerForNextTick(Lambda);
		}
		*/
		return;
	}
	
	if (!Bango::IsComponentInEditedLevel(this))
	{
		return;
	}
	
	// If it already has a Guid, it must have been a copy-paste.
	if (Script.Guid.IsValid())
	{
		FBangoEditorDelegates::OnScriptContainerDuplicated.Broadcast(this, &Script);
	}
	else
	{
		FBangoEditorDelegates::OnScriptContainerCreated.Broadcast(this, &Script);
	}
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	PrintState("OnComponentDestroyed");
	
	// This flag seems to be set when the editor destroys the component, e.g. it is unloaded by world partition. It isn't set when you delete the component. 	
	if (HasAllFlags(RF_BeginDestroyed))
	{
		Super::OnComponentDestroyed(bDestroyingHierarchy);
		return;
	}
		
	// If we are a default actor component, we will always exist on the actor and the only time we'll be truly deleted is when the whole actor hierachy is being deleted
	if (CreationMethod != EComponentCreationMethod::Instance && !bDestroyingHierarchy)
	{
		Super::OnComponentDestroyed(bDestroyingHierarchy);
		return;
	}
	
	if (Bango::IsComponentInEditedLevel(this))
	{
		if (Script.Guid.IsValid())
		{
			// Moves handling over to an editor module
			FBangoEditorDelegates::OnScriptContainerDestroyed.Broadcast(this, &Script);
		}
	}
	
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::PreSave(FObjectPreSaveContext SaveContext)
{
	if (!IsRunningCommandlet())
	{
		//Script.ForceSave();
	}
	
	Super::PreSave(SaveContext);
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::PostEditImport()
{
	Super::PostEditImport();
	
	UE_LOG(LogBango, Display, TEXT("PostEditImport"));
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	if (Bango::IsComponentInEditedLevel(this))
	{
		FBangoEditorDelegates::OnScriptContainerDuplicated.Broadcast(this, &Script);
	}
}

void UBangoScriptComponent::PostLoad()
{
	Super::PostLoad();
}

void UBangoScriptComponent::PostLoadSubobjects(FObjectInstancingGraph* OuterInstanceGraph)
{
	Super::PostLoadSubobjects(OuterInstanceGraph);
}

void UBangoScriptComponent::PostInitProperties()
{
	Super::PostInitProperties();
}

void UBangoScriptComponent::PostApplyToComponent()
{
	Super::PostApplyToComponent();
	
	// If it already has a Guid, it must have been a copy-paste.
	if (Script.Guid.IsValid())
	{
		FBangoEditorDelegates::OnScriptContainerDuplicated.Broadcast(this, &Script);
	}
	else
	{
		FBangoEditorDelegates::OnScriptContainerCreated.Broadcast(this, &Script);
	}
}

void UBangoScriptComponent::PostReloadConfig(class FProperty* PropertyThatWasLoaded)
{
	Super::PostReloadConfig(PropertyThatWasLoaded);
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::UnsetScript()
{
	Modify();
	Script.Unset();
	
	UScriptStruct* ScriptContainerStruct = FBangoScriptContainer::StaticStruct();
	FProperty* ScriptClassProperty = ScriptContainerStruct->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FBangoScriptContainer, ScriptClass));
	
	FPropertyChangedEvent PostEvent2(ScriptClassProperty);
	
	PostEditChangeProperty(PostEvent2);
		
	if (!MarkPackageDirty())
	{
		UE_LOG(LogBlueprint, Error, TEXT("Could not mark the actor package dirty?"));
	}
}

void UBangoScriptComponent::OnRename()
{
	if (HasAnyFlags(RF_MirroredGarbage))
	{
		return;
	}
	
	UBangoScriptBlueprint* Blueprint = UBangoScriptBlueprint::GetBangoScriptBlueprintFromClass(Script.ScriptClass);

	if (Blueprint)
	{
		Blueprint->UpdateAutoName(this);
	}
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::Run()
{
	UObject* ThisInput = nullptr;
		
#if 0
	switch (ThisArg)
	{
		case EBangoScriptComponent_ThisArg::OwnerActor:
		{
			ThisInput = GetOwner();
			break;
		}
		case EBangoScriptComponent_ThisArg::ScriptComponent:
		{
			ThisInput = this;
			break;
		}
		default:
		{
			checkNoEntry();
		}
	}
#endif
	ThisInput = GetOwner();
		
	UBangoScript::RunScript(Script.ScriptClass, ThisInput);
}

FGuid UBangoScriptComponent::GetScriptGuid() const
{
	return Script.Guid;
}
#endif

#if WITH_EDITOR
UBangoScriptBlueprint* UBangoScriptComponent::GetScriptBlueprint() const
{
	return Cast<UBangoScriptBlueprint>(UBlueprint::GetBlueprintFromClass(Script.ScriptClass));
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::SetScriptBlueprint(UBangoScriptBlueprint* Blueprint)
{
	if (!Blueprint->GeneratedClass->IsChildOf(UBangoScript::StaticClass()))
	{
		UE_LOG(LogBango, Error, TEXT("Tried to set blueprint but the blueprint given was not a UBangoScriptInstance!"));
		return;
	}
	
	Modify();
	(void)MarkPackageDirty();
	Script.ScriptClass = Blueprint->GeneratedClass;
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::PostEditUndo(TSharedPtr<ITransactionObjectAnnotation> TransactionAnnotation)
{
	Super::Super::PostEditUndo(TransactionAnnotation);
}
#endif
