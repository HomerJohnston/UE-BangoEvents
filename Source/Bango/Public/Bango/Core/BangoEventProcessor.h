#pragma once

#include "BangoEventProcessor.generated.h"

// PROCESSOR BASE
// ================================================================================================

UCLASS(Abstract)
class UBangoEventProcessor : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> Instigators;
	
protected:
	ABangoEvent* GetEvent() const;

public:
	virtual void Initialize() {};

	virtual bool ActivateFromTrigger(UObject* NewInstigator) { return false; };
	
	virtual void StartActions(UObject* NewInstigator);;

	virtual void DeactivateFromTrigger(UObject* OldInstigator) {};
	
	virtual void StopActions(UObject* OldInstigator);

	int32 GetInstigatorsNum();
};

// BANG PROCESSOR
// ================================================================================================

UCLASS()
class UBangoEventProcessor_Bang : public UBangoEventProcessor
{
	GENERATED_BODY()

public:
	bool ActivateFromTrigger(UObject* NewInstigator) override;
	
	void DeactivateFromTrigger(UObject* OldInstigator) override;
};

// TOGGLE PROCESSOR
// ================================================================================================

UCLASS()
class UBangoEventProcessor_Toggle : public UBangoEventProcessor
{
	GENERATED_BODY()
	
public:
	bool ActivateFromTrigger(UObject* NewInstigator) override;
	
	void DeactivateFromTrigger(UObject* OldInstigator) override;
};

// INSTANCED PROCESSOR
// ================================================================================================

UCLASS()
class UBangoEventProcessor_Instanced : public UBangoEventProcessor
{
	GENERATED_BODY()

private:
	UPROPERTY(Transient)
	TMap<TObjectPtr<UObject>, FBangoEventInstigatorActions> InstancedActionsPerInstigator;

public:
	void Initialize() override;
	
	bool ActivateFromTrigger(UObject* NewInstigator) override;
	
	void StartActions(UObject* NewInstigator) override;

	void DeactivateFromTrigger(UObject* OldInstigator) override;
	
	void StopActions(UObject* OldInstigator) override;
};