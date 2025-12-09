#pragma once

#include "EditorValidatorBase.h"

#include "BangoScriptValidator.generated.h"

class UBangoScript;

/** IsDataValid of a UBangoScriptObject will cause this to run. */
UCLASS()
class UBangoScriptValidator : public UEditorSubsystem
{
	GENERATED_BODY()
	
public:
	
	void Initialize(FSubsystemCollectionBase& Collection) override;
	
	static EDataValidationResult IsScriptDataValid(class FDataValidationContext& Context, const UBangoScript* ScriptObject); 
};