// Copyright Ghost Pepper Games, Inc. All Rights Reserved.

#include "Puny/Editor/PlungerComponent.h"

#include "Bango/Settings/BangoDevSettings.h"
#include "Bango/Utility/Log.h"
#include "Puny/Core/EventComponent.h"
#include "Puny/Editor/PlungerSceneProxy.h"

UPunyPlungerComponent::UPunyPlungerComponent()
{
	bIsEditorOnly = true;
	bHiddenInGame = true;
	bUseEditorCompositing = true;
	SetGenerateOverlapEvents(false);

	UPrimitiveComponent::SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	FAutoConsoleVariableSink CVarSink(FConsoleCommandDelegate::CreateUObject(this, &ThisClass::OnCvarChange));
}

FPrimitiveSceneProxy* UPunyPlungerComponent::CreateSceneProxy()
{
	UE_LOG(Bango, Display, TEXT("UPunyPlungerComponent::CreateSceneProxy"));

	return new FPunyPlungerSceneProxy(this);
}

#if WITH_EDITOR
bool UPunyPlungerComponent::ComponentIsTouchingSelectionBox(const FBox& InSelBox, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const
{
	return Super::ComponentIsTouchingSelectionBox(InSelBox, bConsiderOnlyBSP, bMustEncompassEntireComponent);
}
#endif

#if WITH_EDITOR
bool UPunyPlungerComponent::ComponentIsTouchingSelectionFrustum(const FConvexVolume& InFrustum, const bool bConsiderOnlyBSP, const bool bMustEncompassEntireComponent) const
{
	return Super::ComponentIsTouchingSelectionFrustum(InFrustum, bConsiderOnlyBSP, bMustEncompassEntireComponent);
}
#endif

void UPunyPlungerComponent::BeginPlay()
{
	Super::BeginPlay();

	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>(); 
	
	SetHiddenInGame(!DevSettings->GetShowEventsInGame());
}

FBoxSphereBounds UPunyPlungerComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(FBox(FVector(0,-1,-80),FVector(1.0 * 80 * 3.0f,80,80))).TransformBy(LocalToWorld);
}

#if WITH_EDITOR
FLinearColor UPunyPlungerComponent::GetColor()
{
	return GetEventComponent()->GetDisplayColor();
}
#endif

bool UPunyPlungerComponent::GetIsPushed()
{
	if (!IsValid(GetWorld()) || !GetWorld()->IsGameWorld())
	{
		return false;
	}

	UPunyEvent* Event = GetEvent();
	
	if (!IsValid(Event))
	{
		return false;
	}
	
	return Event->GetIsPlungerPushed();
}

void UPunyPlungerComponent::OnCvarChange()
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

UPunyEventComponent* UPunyPlungerComponent::GetEventComponent()
{
	return Cast<UPunyEventComponent>(GetOuter());
}

UPunyEvent* UPunyPlungerComponent::GetEvent()
{
	return GetEventComponent()->GetEvent();
}

void UPunyPlungerComponent::CreateRenderState_Concurrent(FRegisterComponentContext* Context)
{
	Super::CreateRenderState_Concurrent(Context);
	
	SendRenderDynamicData_Concurrent();
}

void UPunyPlungerComponent::SendRenderDynamicData_Concurrent()
{
	Super::SendRenderDynamicData_Concurrent();
	
	if (!SceneProxy)
	{
		return;
	}
	
	FPunyPlungerDynamicData* DynamicData = new FPunyPlungerDynamicData();
	DynamicData->Color = GetColor();
	DynamicData->bPlungerPushed = GetIsPushed();
	
	FPunyPlungerSceneProxy* PlungerSceneProxy = static_cast<FPunyPlungerSceneProxy*>(SceneProxy);

	UE_LOG(Bango, Display, TEXT("Sending updated dynamic data: %s, %i"), *DynamicData->Color.ToString(), (int32)DynamicData->bPlungerPushed);
	
	ENQUEUE_RENDER_COMMAND(FSendPlungerDynamicData)([PlungerSceneProxy, DynamicData](FRHICommandListImmediate& RHICmdList)
	{
		PlungerSceneProxy->SetDynamicData_RenderThread(DynamicData);
	});
}
