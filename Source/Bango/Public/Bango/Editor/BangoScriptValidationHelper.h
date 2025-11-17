#pragma once

#include "BangoScriptValidationHelper.generated.h"

#if WITH_EDITOR

UCLASS()
class UBangoScriptValidationHelper : public UEditorSubsystem
{
	GENERATED_BODY()
	
	friend class UBangoScriptObject;
	friend class UBangoScriptValidator;
	
protected:
	TDelegate<EDataValidationResult(class FDataValidationContext& Context, const UBangoScriptObject* ScriptObject)> OnScriptRequestValidation;
};

#endif