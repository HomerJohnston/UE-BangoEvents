#pragma once

#include "Bango/Private/Bango/Editor/BangoDebugDrawServiceBase.h"

#include "BangoActorIDComponent.generated.h"

UCLASS(HideCategories=("Navigation", "Tags", "Activation", "AssetUserData"), meta = (BlueprintSpawnableComponent))
class BANGO_API UBangoActorIDComponent : public UActorComponent, public FBangoDebugDrawServiceBase
{
	GENERATED_BODY()

public:
	UBangoActorIDComponent();
	
public:
	FName GetBangoName() const { return Name; }
	
	const FGuid& GetBangoGuid() const { return Guid; }
	
protected:
	UPROPERTY(EditAnywhere, NonPIEDuplicateTransient)
	FName Name;
	
	UPROPERTY(VisibleAnywhere, NonPIEDuplicateTransient)
	FGuid Guid;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, meta = (UIMin = -200, UIMax = 500, Delta = 10))
	float LabelHeight = 200.0f;
	
	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> IconTexture;
#endif
	
	void PostLoad() override;
	
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
#if WITH_EDITOR
public:
	void SetActorID(FName NewID);

	void OnRegister() override;

	void OnUnregister() override;
	
	void DebugDrawEditor(UCanvas* Canvas, FVector ScreenLocation, float Alpha) const override;
	
	void DebugDrawGame(UCanvas* Canvas, FVector ScreenLocation, float Alpha) const override;
	
	float GetLabelHeight() const override { return LabelHeight; }
#endif
};
