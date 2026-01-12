#pragma once

#include "Bango/Debug/BangoDebugDrawServiceBase.h"
#include "Components/ActorComponent.h"

#include "BangoActorIDComponent.generated.h"

UCLASS(HideCategories=("Navigation", "Tags", "Activation", "AssetUserData"), meta = (BlueprintSpawnableComponent))
class BANGO_API UBangoActorIDComponent : public UActorComponent
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
	void PostInitProperties() override;
	
	void PostReinitProperties() override;
	
	void PostDuplicate(bool bDuplicateForPIE) override;
	
	void PostLoad() override;

	void PostLoadSubobjects(FObjectInstancingGraph* OuterInstanceGraph) override;
	
	void PostLinkerChange() override;

	void Serialize(FArchive& Ar) override;
	
	bool Rename(const TCHAR* NewName = nullptr, UObject* NewOuter = nullptr, ERenameFlags Flags = (0)) override;
	
	void PostEditImport() override;
	
	void PostApplyToComponent() override;
	
	void CheckForErrors() override;
	
	void InitializeComponent() override;
	
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void PrintGuid(const FString& FuncName);
	
#if WITH_EDITOR
private:
	void EnsureValidGuid();
#endif
	
#if WITH_EDITOR
public:
	void SetBangoName(FName NewID);

	void OnRegister() override;

	void OnUnregister() override;

	void OnComponentCreated() override;
	
	void DebugDrawEditor(UCanvas* Canvas, FVector ScreenLocation, float Alpha) const;
	
	void DebugDrawGame(UCanvas* Canvas, FVector ScreenLocation, float Alpha) const;
	
#endif
};
