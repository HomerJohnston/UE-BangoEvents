#include "Bango/Components/BangoScriptComponent.h"

#include "Bango/Core/BangoBlueprintFunctionLibrary.h"
#include "Bango/Core/BangoScript.h"
//#include "Bango/Editor/BangoScriptHelperSubsystem.h"
#include "Bango/Subsystem/BangoScriptSubsystem.h"
#include "Bango/Utility/BangoHelpers.h"
#include "Bango/Utility/BangoLog.h"
#include "UObject/ObjectSaveContext.h"
#include "UObject/PropertyAccessUtil.h"

UBangoScriptComponent::UBangoScriptComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBangoScriptComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (!bPreventStarting)
	{
		UBangoScript::RunScript(Script.ScriptClass, this);
	}
}

#if WITH_EDITOR
void UBangoScriptComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	
	Modify();
	MarkPackageDirty();
	GetOwner()->Modify();
	GetOwner()->MarkPackageDirty();
	
	if (!GEditor->IsPlaySessionInProgress())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UBangoScriptComponent::TrySetGUID);
	}
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if (Bango::IsComponentInEditedLevel(this))
	{
		if (Script.Guid.IsValid())
		{
			//GEditor->GetEditorSubsystem<UBangoScriptHelperSubsystem>()->OnScriptComponentDestroyed.Broadcast(this);
			FBangoEditorDelegates::OnScriptComponentDestroyed.Broadcast(this);
		}
	}
	
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);
	
	if (Script.ScriptClass)
	{
		UBangoScriptBlueprint* Blueprint = Cast<UBangoScriptBlueprint>(UBlueprint::GetBlueprintFromClass(Script.ScriptClass));
		
		if (Blueprint)
		{
			Blueprint->ForceSave();
		}
	}
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::TrySetGUID()
{
	if (!Script.Guid.IsValid())
	{
		Modify();
		MarkPackageDirty();
		GetOwner()->Modify();
		GetOwner()->MarkPackageDirty();
		
		Script.Guid = FGuid::NewGuid();
		
		FBangoEditorDelegates::OnScriptComponentCreated.Broadcast(this, nullptr);
	}
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::UnsetScript()
{
	Modify();
	Script.PrepareForDestroy();
	
	UScriptStruct* ScriptContainerStruct = FBangoScriptContainer::StaticStruct();
	FProperty* ScriptClassProperty = ScriptContainerStruct->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FBangoScriptContainer, ScriptClass));
	
	FPropertyChangedEvent PostEvent2(ScriptClassProperty);
	
	PostEditChangeProperty(PostEvent2);
		
	if (!MarkPackageDirty())
	{
		UE_LOG(LogBlueprint, Error, TEXT("Could not mark the actor package dirty?"));
	}
}
#endif

#if WITH_EDITOR
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
	MarkPackageDirty();
	Script.ScriptClass = Blueprint->GeneratedClass;
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::PostEditUndo(TSharedPtr<ITransactionObjectAnnotation> TransactionAnnotation)
{
	Super::Super::PostEditUndo(TransactionAnnotation);
}
#endif
