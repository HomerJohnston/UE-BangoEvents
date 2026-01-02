#pragma once

#include "Bango/Utility/BangoHelpers.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"

/**  WIP -- works for editor viewport non-PIE debug drawing // TODO implement PIE debug drawing and update this
 * 
 * Usage: 
 * 1) Inherit this onto your AActorComponent class
 * 2) Override OnRegister
 *   - After Super::OnRegister, call BangoDebugDraw_Register<ThisClass>(this);
 * 3) Override OnUnregister
 *   - Before Super::OnUnregister, call BangoDebugDraw_Unregister(this);
 * 4) Override DebugDrawEditor and implement
 * 5) Optionally override GetLabelHeight to return a UPROPERTY or other logic
 */
class FBangoDebugDrawServiceBase
{
#if WITH_EDITOR
public:
	virtual ~FBangoDebugDrawServiceBase() = default;
	
protected:
	template <typename T>
	void BangoDebugDraw_Register(UActorComponent* Component);
	
	void BangoDebugDraw_Unregister(UActorComponent* Component);

	// Gets camera position in worldspace
	void GetCameraPos(UCanvas* Canvas, FVector& CameraPos) const;
	
	// Gets camera position in worldspace
	void GetCameraPos(UCanvas* Canvas, FVector& CameraPos, FVector& CameraDir) const;
	
	// Gets actor position in screen space, returns false if the actor is not visible
	bool GetActorScreenLocation(UCanvas* Canvas, AActor* Actor, FVector& ScreenLocation, float LabelHeight = 0) const; 
	
	float GetNormalizedDistToActor(UCanvas* Canvas, AActor* Actor) const;
	
private:
	FDelegateHandle DebugDrawServiceHandle_Editor;
	FDelegateHandle DebugDrawServiceHandle_Game;
	
	/*
	template <typename T>
	void __RegisterDebugDraw(const bool PIE);
	
	//template <typename T>
	void __UnregisterDebugDraw(const bool PIE);
	*/
	
	void __DebugDrawEditor(UCanvas* Canvas, APlayerController* PlayerController) const;
	
	void __DebugDrawGame(UCanvas* Canvas, APlayerController* PlayerController) const;
	
	void __DebugDrawDispatch(UCanvas* Canvas, TDelegate<void(UCanvas*, FVector, float)> Delegate) const;
	
	virtual void DebugDrawEditor(UCanvas* Canvas, FVector ScreenLocation, float Alpha) const { };
	
	virtual void DebugDrawGame(UCanvas* Canvas, FVector ScreenLocation, float Alpha) const { };
	
protected:
	TWeakObjectPtr<UActorComponent> This = nullptr;
	float MinDistance = 2000.0f;
	float MaxDistance = 2500.0f;
	float DefaultLabelHeight = 200.0f;
	
	// This should be overridden in all user classes, use this to avoid having different labels overlap each other. This is kind of ghetto but it's easy and it works.
	float LabelOffset = 0.0f;
	
	// Override this to return a UPROPERTY value
	virtual float GetLabelHeight() const { return DefaultLabelHeight + LabelOffset; }
#endif
};

// ==============================================

#if WITH_EDITOR
template <typename T>
inline void FBangoDebugDrawServiceBase::BangoDebugDraw_Register(UActorComponent* Component)
{
	check(Component);
	
	if (Component->IsTemplate())
	{
		return;
	}

	This = Component;
	T* TypedThis = Cast<T>(This);
	
	if (Bango::IsComponentInEditedLevel(Component))
	{
		FEditorDelegates::PrePIEEnded.AddWeakLambda(This.Get(), [this, TypedThis] (const bool PIE)
		{
			if (!DebugDrawServiceHandle_Editor.IsValid() && Bango::IsComponentInEditedLevel(TypedThis))
			{
				DebugDrawServiceHandle_Editor = UDebugDrawService::Register(TEXT("Editor"), FDebugDrawDelegate::CreateUObject(TypedThis, &T::__DebugDrawEditor));
			}
		});
	
		FEditorDelegates::PreBeginPIE.AddWeakLambda(This.Get(), [this] (const bool PIE)
		{
			UDebugDrawService::Unregister(DebugDrawServiceHandle_Editor);
			DebugDrawServiceHandle_Editor.Reset();
		});	
		
		DebugDrawServiceHandle_Editor = UDebugDrawService::Register(TEXT("Editor"), FDebugDrawDelegate::CreateUObject(TypedThis, &T::__DebugDrawEditor));
	}
	else if (Component->GetWorld()->IsGameWorld())
	{
		if (!DebugDrawServiceHandle_Game.IsValid())
		{
			DebugDrawServiceHandle_Game = UDebugDrawService::Register(TEXT("Game"), FDebugDrawDelegate::CreateUObject(TypedThis, &T::__DebugDrawGame));
		}
	}
}

// ----------------------------------------------

inline void FBangoDebugDrawServiceBase::BangoDebugDraw_Unregister(UActorComponent* Component)
{
	if (Component->IsTemplate())
	{
		return;
	}

	UDebugDrawService::Unregister(DebugDrawServiceHandle_Editor);
	UDebugDrawService::Unregister(DebugDrawServiceHandle_Game);

	DebugDrawServiceHandle_Editor.Reset();
	DebugDrawServiceHandle_Game.Reset();
}

// ----------------------------------------------

inline void FBangoDebugDrawServiceBase::GetCameraPos(UCanvas* Canvas, FVector& CameraPos) const
{
	check(Canvas);

	FVector DummyDir;
	GetCameraPos(Canvas, CameraPos, DummyDir);
}

inline void FBangoDebugDrawServiceBase::GetCameraPos(UCanvas* Canvas, FVector& CameraPos, FVector& CameraDir) const
{
	check(Canvas);
	
	double X, Y;
	Canvas->GetCenter(X, Y);
	Canvas->Deproject(FVector2D(X, Y), CameraPos, CameraDir);
}

// ----------------------------------------------

inline bool FBangoDebugDrawServiceBase::GetActorScreenLocation(UCanvas* Canvas, AActor* Actor, FVector& ScreenLocation, float LabelHeight) const
{
	check(Canvas);
	check(Actor);
	
	ScreenLocation = Canvas->Project(Actor->GetActorLocation() + FVector(0.0f, 0.0f, LabelHeight), false);
	
	if (ScreenLocation.Z < 0.0f)
	{
		return false;
	}
	
	return true;
}

// ----------------------------------------------

inline float FBangoDebugDrawServiceBase::GetNormalizedDistToActor(UCanvas* Canvas, AActor* Actor) const
{
	check(Canvas);
	check(Actor);
	
	FVector CameraPos;
	GetCameraPos(Canvas, CameraPos);
	
	float MinDistanceSq = FMath::Square(MinDistance);
	float MaxDistanceSq = FMath::Square(MaxDistance);
	float DistanceSq = FVector::DistSquared(CameraPos, Actor->GetActorLocation());
	
	// This isn't linear but I'm OK with that for debug drawing
	float LerpAlpha = FMath::Clamp((DistanceSq - MinDistanceSq) / (MaxDistanceSq - MinDistanceSq), 0.0f, 1.0f);
	
	return 1.0f - LerpAlpha;
}

// ----------------------------------------------

inline void FBangoDebugDrawServiceBase::__DebugDrawEditor(UCanvas* Canvas, APlayerController* PlayerController) const
{
	TDelegate<void(UCanvas*, FVector, float)> Delegate;
	Delegate.BindRaw(this, &FBangoDebugDrawServiceBase::DebugDrawEditor);
	__DebugDrawDispatch(Canvas, Delegate);
}

inline void FBangoDebugDrawServiceBase::__DebugDrawGame(UCanvas* Canvas, APlayerController* PlayerController) const
{
	TDelegate<void(UCanvas*, FVector, float)> Delegate;
	Delegate.BindRaw(this, &FBangoDebugDrawServiceBase::DebugDrawGame);
	__DebugDrawDispatch(Canvas, Delegate);
}

inline void FBangoDebugDrawServiceBase::__DebugDrawDispatch(UCanvas* Canvas, TDelegate<void(UCanvas*, FVector, float)> Delegate) const
{
	check(This.IsValid());
	
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
}

// ----------------------------------------------
#endif