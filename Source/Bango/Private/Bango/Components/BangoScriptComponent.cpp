#include "Bango/Components/BangoScriptComponent.h"

#include "Bango/Core/BangoBlueprintFunctionLibrary.h"
#include "Bango/Core/BangoScriptObject.h"
#include "Bango/Editor/BangoScriptHelperSubsystem.h"
#include "Bango/Subsystem/BangoScriptSubsystem.h"
#include "Bango/Utility/BangoHelpers.h"
#include "Bango/Utility/BangoLog.h"
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
		UBangoScriptInstance::RunScript(Script.ScriptClass, this);
	}
}

#if WITH_EDITOR
void UBangoScriptComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	
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
			GEditor->GetEditorSubsystem<UBangoScriptHelperSubsystem>()->OnScriptComponentDestroyed.Broadcast(this);
		}
	}
	
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::TrySetGUID()
{
	if (!Script.Guid.IsValid())
	{
		Modify();
		Script.Guid = FGuid::NewGuid();
		
		GEditor->GetEditorSubsystem<UBangoScriptHelperSubsystem>()->OnScriptComponentCreated.Broadcast(this);
	}
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::UnsetScript()
{
	Modify();
	Script.PrepareForDestroy();
	
	UScriptStruct* ScriptContainerStruct = FBangoScriptContainer::StaticStruct();
	FProperty* ScriptBlueprintProperty = ScriptContainerStruct->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FBangoScriptContainer, ScriptBlueprint));
	FProperty* ScriptClassProperty = ScriptContainerStruct->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FBangoScriptContainer, ScriptClass));
	
	FPropertyChangedEvent PostEvent1(ScriptBlueprintProperty);
	FPropertyChangedEvent PostEvent2(ScriptClassProperty);
	
	PostEditChangeProperty(PostEvent1);
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
UBlueprint* UBangoScriptComponent::GetScriptBlueprint() const
{
	return Script.ScriptBlueprint;
}
#endif

#if WITH_EDITOR
void UBangoScriptComponent::SetScriptBlueprint(UBlueprint* Blueprint)
{
	if (!Blueprint->GeneratedClass->IsChildOf(UBangoScriptInstance::StaticClass()))
	{
		UE_LOG(LogBango, Error, TEXT("Tried to set blueprint but the blueprint given was not a UBangoScriptInstance!"));
		return;
	}
	
	Script.ScriptBlueprint = Blueprint;
	Script.ScriptClass = Blueprint->GeneratedClass;
	Modify();
}
#endif
