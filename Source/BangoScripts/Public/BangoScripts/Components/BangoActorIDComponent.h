#pragma once

#include "BangoScripts/Debug/BangoDebugDrawServiceBase.h"
#include "Components/ActorComponent.h"

#include "BangoActorIDComponent.generated.h"

/**
 * Bango Actor ID Component MAY become deprecated! It was originally intended to be used for level scripts to be able to look up actors from the world. 
 * However, I since figured out how to create soft pointers to actors in the level scripts which seem to work better. Real-world testing required.
 */
UCLASS(HideCategories=("Navigation", "Tags", "Activation", "AssetUserData"), meta = (BlueprintSpawnableComponent))
class BANGOSCRIPTS_API UBangoActorIDComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBangoActorIDComponent();
	
public:
	FName GetBangoName() const { return BangoName; }
	
protected:
	UPROPERTY(EditAnywhere, NonPIEDuplicateTransient, TextExportTransient)
	FName BangoName;
	
	UPROPERTY(EditAnywhere, NonPIEDuplicateTransient, TextExportTransient)
	FGuid BangoGuid;
	
	UPROPERTY(EditAnywhere, NonPIEDuplicateTransient, TextExportTransient)
	FGuid UnusedGuid;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> IconTexture;
#endif
	void PostDuplicate(bool bDuplicateForPIE) override;
	
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
#if WITH_EDITOR
protected:
	void PrintGuid(const FString& FuncName);
	
	void EnsureValidGuid();
#endif
	
#if WITH_EDITOR
public:
	void SetBangoName(FName NewID);

	void OnRegister() override;

	void OnUnregister() override;

	void DebugDrawEditor(UCanvas* Canvas, FVector ScreenLocation, float Alpha) const;
	
	void DebugDrawGame(UCanvas* Canvas, FVector ScreenLocation, float Alpha) const;
	
#endif
};