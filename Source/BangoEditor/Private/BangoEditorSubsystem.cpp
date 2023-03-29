#include "BangoEditorSubsystem.h"
#include "Bango/Core/BangoEvent.h"
#include "Bango/Subsystems/BangoEngineSubsystem.h"
#include "Bango/Settings/BangoDevSettings.h"

void UBangoEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UBangoEngineSubsystem* EngineSubsystem = GEngine->GetEngineSubsystem<UBangoEngineSubsystem>();

	EngineSubsystem->OnEventAltered.AddUObject(this, &UBangoEditorSubsystem::OnBangoEventAltered);
}

void UBangoEditorSubsystem::Tick(float DeltaTime)
{
	
}

TStatId UBangoEditorSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UBangoEditorSubsystem, STATGROUP_Tickables)
}

void UBangoEditorSubsystem::OnBangoEventAltered(ABangoEvent* Event)
{
	TryApplyMesh(Event);
	TryApplyMaterial(Event);
}

void UBangoEditorSubsystem::TryApplyMesh(ABangoEvent* Event)
{
	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();

	TSoftObjectPtr<UStaticMesh> Mesh = nullptr;
	
	if (Event->HasCurrentState(EBangoEventState::Active))
	{
		Mesh = DevSettings->ActiveMesh;
	}
	else
	{
		Mesh = DevSettings->NormalMesh;
	}
	
	if (Mesh.IsNull())
	{
		// TODO LogTemp
		UE_LOG(LogTemp, Warning, TEXT("No editor mesh is selected, will not apply editor mesh to Bango events"));
		return;
	}
	// TODO: what happens if user closes engine or loads different level during async load???
	
	if (Mesh.IsPending())
	{
		FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ThisClass::ApplyLoadedMesh, Event, Mesh);
		StreamableManager.RequestAsyncLoad(Mesh.ToSoftObjectPath(), Delegate);
	}
	else
	{
		ApplyLoadedMesh(Event, Mesh);
	}
}

void UBangoEditorSubsystem::ApplyLoadedMesh(ABangoEvent* Event, TSoftObjectPtr<UStaticMesh> LoadedObject)
{
	if (!LoadedObject.IsValid())
	{
		// TODO LogTemp
		UE_LOG(LogTemp, Warning, TEXT("Failed to load editor mesh! This should never happen."));
		return;
	}

	Event->SetDebugMesh(LoadedObject.Get());
}

void UBangoEditorSubsystem::TryApplyMaterial(ABangoEvent* Event)
{
	const UBangoDevSettings* DevSettings = GetDefault<UBangoDevSettings>();
	
	TSoftObjectPtr<UMaterialInstance> Material;
	
	if (Event->HasCurrentState(EBangoEventState::Active))
	{
		Material = DevSettings->StateMaterials[(uint8)EBangoEventState::Active];
	}
	else if (Event->HasCurrentState(EBangoEventState::Expired))
	{
		Material = DevSettings->StateMaterials[(uint8)EBangoEventState::Expired];		
	}
	else if (Event->HasCurrentState(EBangoEventState::Frozen))
	{
		Material = DevSettings->StateMaterials[(uint8)EBangoEventState::Frozen];		
	}
	else if (Event->HasCurrentState(EBangoEventState::StartDelay))
	{
		Material = DevSettings->StateMaterials[(uint8)EBangoEventState::StartDelay];		
	}
	else
	{
		Material = DevSettings->StateMaterials[(uint8)EBangoEventState::Normal];
	}
	
	if (Material.IsPending())
	{
		FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ThisClass::ApplyLoadedMaterial, Event, Material);
		StreamableManager.RequestAsyncLoad(Material.ToSoftObjectPath(), Delegate);
	}
	else
	{
		ApplyLoadedMaterial(Event, Material);
	}
}


void UBangoEditorSubsystem::ApplyLoadedMaterial(ABangoEvent* Event, TSoftObjectPtr<UMaterialInstance> LoadedMaterialInstance)
{
	if (!LoadedMaterialInstance.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load editor material! This should never happen."));
		return;
	}

	Event->SetDebugMeshMaterial(LoadedMaterialInstance.Get());
}
