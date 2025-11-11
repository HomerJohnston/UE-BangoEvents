#pragma once

#include "BangoActorIDComponent.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class BANGO_API UBangoActorIDComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FName GetActorID() const { return ActorID; }
	
protected:
	UPROPERTY(EditAnywhere)
	FName ActorID;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, meta = (UIMin = -200, UIMax = 500, Delta = 10))
	float LabelHeightAdjustment = 0.0f;
#endif
	
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
#if WITH_EDITOR
public:
	void SetActorID(FName NewID);

	FDelegateHandle DebugDrawService;
	
	void OnRegister() override;

	void UnregisterDebugDraw(const bool PIE);
	
	void ReregisterDebugDraw(const bool PIE);
	
	void OnUnregister() override;
	
	void DebugDrawEditor(UCanvas* Canvas, APlayerController* PlayerController) const;
#endif
};
