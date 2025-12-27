#pragma once

#include "BangoActorIDComponent.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class BANGO_API UBangoActorIDComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBangoActorIDComponent();
	
public:
	FName GetBangoName() const { return Name; }
	
	const FGuid& GetBangoGuid() const { return Guid; }
	
protected:
	UPROPERTY(EditAnywhere)
	FName Name;
	
	UPROPERTY(VisibleAnywhere)
	FGuid Guid;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, meta = (UIMin = -200, UIMax = 500, Delta = 10))
	float LabelHeightAdjustment = 0.0f;
	
	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> IconTexture;
#endif
	
	void PostInitProperties() override;
	
	void PostLoad() override;
	
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
	
	void OnComponentDestroyed(bool bDestroyingHierarchy) override;
#endif
	
	bool IsBeingEditorDeleted() const;
};
