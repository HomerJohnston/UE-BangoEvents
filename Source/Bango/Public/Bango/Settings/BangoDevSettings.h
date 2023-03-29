#pragma once

#include "Engine/DeveloperSettings.h"
#include "Bango/Core/BangoEvent.h"

#include "BangoDevSettings.generated.h"

UCLASS(Config = Engine, DefaultConfig, DisplayName="Bango Event System")
class BANGO_API UBangoDevSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	//TSoftObjectPtr<UStaticMesh> 
	UPROPERTY(Category="Debug|Mesh", EditAnywhere, Config)
	TSoftObjectPtr<UStaticMesh> NormalMesh;

	UPROPERTY(Category="Debug|Mesh", EditAnywhere, Config)
	TSoftObjectPtr<UStaticMesh> ActiveMesh;
	
	UPROPERTY(Category="Debug|Materials", EditAnywhere, Config, meta=(ArraySizeEnum="EBangoEventState"))
	TSoftObjectPtr<UMaterialInstance> StateMaterials[EBangoEventState::MAX];
};
