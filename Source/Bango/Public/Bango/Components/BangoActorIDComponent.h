#pragma once

#include "BangoActorIDComponent.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class UBangoActorIDComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FName GetActorID() const { return ActorID; }
	
protected:
	UPROPERTY(EditAnywhere)
	FName ActorID;

	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
#if WITH_EDITOR
public:
	void SetActorID(FName NewID) { ActorID = NewID; }
	
	FDelegateHandle DebugDrawService;
	
	void OnRegister() override;

	void UnregisterDebugDraw(const bool PIE);
	
	void ReregisterDebugDraw(const bool PIE);
	
	void OnUnregister() override;
	
	void DebugDrawEditor(UCanvas* Canvas, APlayerController* PlayerController) const;
#endif
};
