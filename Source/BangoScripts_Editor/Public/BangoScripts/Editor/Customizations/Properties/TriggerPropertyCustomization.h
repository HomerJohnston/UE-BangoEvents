#pragma once

#include "ActionTriggerPropertyCustomizationBase.h"
#include "IPropertyTypeCustomization.h"

class FBangoTriggerPropertyCustomization : public FBangoActionTriggerPropertyCustomizationBase
{
public:
	FBangoTriggerPropertyCustomization();
	
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
};

