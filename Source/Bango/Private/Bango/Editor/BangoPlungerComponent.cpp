// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Bango/Editor/BangoPlungerComponent.h"

#include "Bango/Settings/BangoDevSettings.h"
#include "Bango/Core/BangoEventComponent.h"
#include "Bango/Editor/BangoPlungerSceneProxy.h"

UBangoPlungerComponent::UBangoPlungerComponent()
{
	bIsEditorOnly = true;
	bHiddenInGame = true;
	bUseEditorCompositing = true;
	SetGenerateOverlapEvents(false);

	UPrimitiveComponent::SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	FAutoConsoleVariableSink CVarSink(FConsoleCommandDelegate::CreateUObject(this, &ThisClass::OnCvarChange));
}

FPrimitiveSceneProxy* UBangoPlungerComponent::CreateSceneProxy()
{
	return new FBangoPlungerSceneProxy(this);
}

#if WITH_EDITOR
bool UBangoPlungerComponent::ComponentIsTouchingSelectionBox(const FBox& InSelBox, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const
{
	return Super::ComponentIsTouchingSelectionBox(InSelBox, bConsiderOnlyBSP, bMustEncompassEntireComponent);
}
#endif

#if WITH_EDITOR
bool UBangoPlungerComponent::ComponentIsTouchingSelectionFrustum(const FConvexVolume& InFrustum, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const
{
	return Super::ComponentIsTouchingSelectionFrustum(InFrustum, bConsiderOnlyBSP, bMustEncompassEntireComponent);
}
#endif

void UBangoPlungerComponent::BeginPlay()
{
	Super::BeginPlay();

	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>(); 
	
	SetHiddenInGame(!DevSettings->GetShowEventsInGame());
}

FBoxSphereBounds UBangoPlungerComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(FBox(FVector(0,-1,-80),FVector(1.0 * 80 * 3.0f,80,80))).TransformBy(LocalToWorld);
}

#if WITH_EDITOR
FLinearColor UBangoPlungerComponent::GetColor()
{
	return GetEventComponent()->GetDisplayColor();
}
#endif

#if WITH_EDITOR
bool UBangoPlungerComponent::GetIsActive()
{
	if (!IsValid(GetWorld()) || !GetWorld()->IsGameWorld())
	{
		return false;
	}
	
	UBangoEvent* Event = GetEvent();
	
	if (!IsValid(Event))
	{
		return false;
	}
	
	return Event->GetIsActive();
}
#endif

void UBangoPlungerComponent::OnCvarChange()
{
	AActor* OwnerActor = GetOwner();

	if (!IsValid(OwnerActor))
	{
		return;
	}

	UWorld* World = OwnerActor->GetWorld();
	
	if (!IsValid(World) || !World->IsGameWorld())
	{
		return;
	}

	const IConsoleVariable* ShowInGameCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("Bango.ShowEventsInGame"));

	bool bNewHiddenInGame = !ShowInGameCVar->GetBool();
	
	SetHiddenInGame(bNewHiddenInGame);
}

UBangoEventComponent* UBangoPlungerComponent::GetEventComponent()
{
	return Cast<UBangoEventComponent>(GetOuter());
}

UBangoEvent* UBangoPlungerComponent::GetEvent()
{
	return GetEventComponent()->GetEvent();
}

void UBangoPlungerComponent::CreateRenderState_Concurrent(FRegisterComponentContext* Context)
{
	Super::CreateRenderState_Concurrent(Context);

#if WITH_EDITOR
	SendRenderDynamicData_Concurrent();
#endif
}

void UBangoPlungerComponent::SendRenderDynamicData_Concurrent()
{
	Super::SendRenderDynamicData_Concurrent();
	
#if WITH_EDITOR
	if (!SceneProxy)
	{
		return;
	}

	double WorldTime = GetWorld()->GetTimeSeconds();
	
	FBangoPlungerDynamicData* DynamicData = new FBangoPlungerDynamicData();

	bool bInPlay = GetWorld()->IsGameWorld();
	
	if (IsValid(GetEventComponent()))
	{
		DynamicData->BaseColor = GetEventComponent()->GetDisplayColor();
		DynamicData->bIsDisabled = GetEventComponent()->GetIsDisabled();
		DynamicData->bIsFrozen = bInPlay ? GetEventComponent()->GetIsFrozen() : GetEventComponent()->GetStartsFrozen();
	}
	
	if (IsValid(GetEvent()))
	{
		DynamicData->ActivationTime = FPlatformTime::Seconds() - (WorldTime - GetEvent()->GetLastActivateTime());
		DynamicData->DeactivationTime = FPlatformTime::Seconds() - (WorldTime - GetEvent()->GetLastDeactivateTime());

		DynamicData->bIsExpired = GetEvent()->GetIsExpired();
		DynamicData->bIsActive = GetEvent()->GetIsActive();
	}
	
	FBangoPlungerSceneProxy* PlungerSceneProxy = static_cast<FBangoPlungerSceneProxy*>(SceneProxy);

	ENQUEUE_RENDER_COMMAND(FSendPlungerDynamicData)([PlungerSceneProxy, DynamicData](FRHICommandListImmediate& RHICmdList)
	{
		PlungerSceneProxy->SetDynamicData_RenderThread(DynamicData);
	});
#endif
}
