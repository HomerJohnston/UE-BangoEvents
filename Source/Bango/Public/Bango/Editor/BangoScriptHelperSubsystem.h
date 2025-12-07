#pragma once

#if WITH_EDITOR
#include "BangoScriptHelperSubsystem.generated.h"
#endif

class UBangoScriptComponent;

#if WITH_EDITOR
UCLASS()
class BANGO_API UBangoScriptHelperSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
	
public:
	
	static TMulticastDelegate<void(UBangoScriptComponent*)> OnScriptComponentCreated;
	static TMulticastDelegate<void(UBangoScriptComponent*)> OnScriptComponentDestroyed;
};
#endif