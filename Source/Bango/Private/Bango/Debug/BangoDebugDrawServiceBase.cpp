
#if 0
#include "Bango/Debug/BangoDebugDrawServiceBase.h"

// ----------------------------------------------

#if WITH_EDITOR
void FBangoDebugDrawServiceBase::BangoDebugDraw_Unregister(UActorComponent* Component)
{
	if (Component->IsTemplate())
	{
		return;
	}

	FBangoEditorDelegates::BangoDebugDrawEditor.RemoveAll(this);
	FBangoEditorDelegates::BangoDebugDrawGame.RemoveAll(this);
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void FBangoDebugDrawServiceBase::__DebugDrawEditor(UCanvas* Canvas, APlayerController* PlayerController) const
{
	TDelegate<void(UCanvas*, FVector, float)> Delegate;
	Delegate.BindRaw(this, &FBangoDebugDrawServiceBase::DebugDrawEditor);
	__DebugDrawDispatch(Canvas, Delegate);
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void FBangoDebugDrawServiceBase::__DebugDrawGame(UCanvas* Canvas, APlayerController* PlayerController) const
{
	TDelegate<void(UCanvas*, FVector, float)> Delegate;
	Delegate.BindRaw(this, &FBangoDebugDrawServiceBase::DebugDrawGame);
	__DebugDrawDispatch(Canvas, Delegate);
}
#endif

// ----------------------------------------------

#if WITH_EDITOR
void FBangoDebugDrawServiceBase::__DebugDrawDispatch(UCanvas* Canvas, TDelegate<void(UCanvas*, FVector, float)> Delegate) const
{
	/*
	check(IsValid());
	
	const UWorld* World = This->GetWorld();
	if (!IsValid(World)) return;

	AActor* Actor = This->GetOwner();
	if (!IsValid(Actor)) return;

	FVector ScreenLocation;
	if (!GetActorScreenLocation(Canvas, Actor, ScreenLocation, GetLabelHeight())) return;

	FVector WorldCameraPos;
	FVector WorldCameraDir;
	GetCameraPos(Canvas, WorldCameraPos, WorldCameraDir);

	// TODO move to editor settings
	const float Alpha = GetNormalizedDistToActor(Canvas, Actor);

	if (Alpha > KINDA_SMALL_NUMBER)
	{
		Delegate.Execute(Canvas, ScreenLocation, Alpha);
	}
	*/
}
#endif

// ----------------------------------------------
#endif