// Copyright © 2024 Botanibots Team. All rights reserved.


#include "ItemEditorApp.h"

#include "EngineUtils.h"
#include "GameplayInventoryEditor.h"
#include "Definitions/GameplayEquipmentDefinition.h"
#include "Definitions/GameplayInventoryItemDefinition.h"
#include "Details/DetailsCustomization_Item.h"
#include "Modes/ApplicationMode_Equipment.h"
#include "Modes/ApplicationMode_Item.h"
#include "Pickup/PickupProxyActor.h"
#include "Subsystems/UnrealEditorSubsystem.h"
#include "Toolbar/ItemEditorToolbar.h"

FItemEditorApp::FItemEditorApp()
	: FWorkflowCentricApplication()
	, FEditorUndoClient()
{
	ItemDefinition = nullptr;
	EquipmentDefinition = nullptr;
}

void FItemEditorApp::InitItemEditorApp(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* Object)
{
	// Get all assets that can be edited
	UGameplayInventoryItemDefinition* ItemDef = Cast<UGameplayInventoryItemDefinition>(Object);
	UGameplayEquipmentDefinition* EquipmentDef = Cast<UGameplayEquipmentDefinition>(Object);

	if (ItemDef != nullptr)
	{
		ItemDefinition = ItemDef;

		if (ItemDef->EquipmentDefinition)
		{
			EquipmentDefinition = ItemDef->EquipmentDefinition;
		}
	}
	else if (EquipmentDef != nullptr)
	{
		EquipmentDefinition = EquipmentDef;
	}

	TSharedPtr<FItemEditorApp> ThisPtr(SharedThis(this));

	// Get all objects that can be edited
	TArray<UObject*> ObjectsToEdit;
	if (ItemDefinition)
	{
		ObjectsToEdit.Add(ItemDefinition);
	}
	if (EquipmentDefinition)
	{
		ObjectsToEdit.Add(EquipmentDefinition);
	}

	if (!ToolbarBuilder.IsValid())
	{
		ToolbarBuilder = MakeShareable(new FItemEditorToolbar(ThisPtr));
	}

	// If we're already editing an object, no need to recreate a new editor from scratch but update the existing one
	const TArray<UObject*>* EditedObjects = GetObjectsCurrentlyBeingEdited();
	if (EditedObjects == nullptr || EditedObjects->Num() == 0)
	{
		InitAssetEditor(Mode, InitToolkitHost, ItemEditorIDs::ItemEditorApp, FTabManager::FLayout::NullLayout, true, true, ObjectsToEdit);

		// Create the property view
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		FDetailsViewArgs DetailsViewArgs;
		{
			DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
			DetailsViewArgs.bUpdatesFromSelection = false;
			DetailsViewArgs.bCustomNameAreaLocation = false;
			DetailsViewArgs.bLockable = false;
			DetailsViewArgs.bAllowSearch = true;
			DetailsViewArgs.bHideSelectionTip = true;
			DetailsViewArgs.bShowOptions = true;
			DetailsViewArgs.bShowModifiedPropertiesOption = true;
			DetailsViewArgs.bShowScrollBar = true;
			DetailsViewArgs.NotifyHook = this;
		}

		// Customization for the details view
		DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
		FOnGetDetailCustomizationInstance DetailCustomization =
			FOnGetDetailCustomizationInstance::CreateStatic(&FDetailsCustomization_Item::MakeInstance, false);
		
		DetailsView->RegisterInstancedCustomPropertyLayout(UGameplayInventoryItemDefinition::StaticClass(), DetailCustomization);
		DetailsView->RegisterInstancedCustomPropertyLayout(UGameplayEquipmentDefinition::StaticClass(), DetailCustomization);

		// Customization for the fragment details view
		FragmentDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
		FOnGetDetailCustomizationInstance FragmentDetailCustomization =
			FOnGetDetailCustomizationInstance::CreateStatic(&FDetailsCustomization_Item::MakeInstance, true);

		FragmentDetailsView->RegisterInstancedCustomPropertyLayout(UGameplayInventoryItemDefinition::StaticClass(), FragmentDetailCustomization);
		FragmentDetailsView->RegisterInstancedCustomPropertyLayout(UGameplayEquipmentDefinition::StaticClass(), FragmentDetailCustomization);

		// Set the object to edit
		if (ItemDefinition)
		{
			DetailsView->SetObject(ItemDefinition);
			FragmentDetailsView->SetObject(ItemDefinition);
		}
		else if (EquipmentDefinition)
		{
			DetailsView->SetObject(EquipmentDefinition);
			FragmentDetailsView->SetObject(EquipmentDefinition);
		}

		// Add the application modes
		AddApplicationMode(ItemEditorIDs::Modes::ItemMode, MakeShareable(new FApplicationMode_Item(ThisPtr)));
		AddApplicationMode(ItemEditorIDs::Modes::EquipmentMode, MakeShareable(new FApplicationMode_Equipment(ThisPtr)));
	}
	else
	{
		for (UObject* ObjToEdit : ObjectsToEdit)
		{
			AddEditingObject(ObjToEdit);
		}
	}


	// Update the mode
	if (ItemDef)
	{
		SetCurrentMode(ItemEditorIDs::Modes::ItemMode);
	}
	else if (EquipmentDef)
	{
		SetCurrentMode(ItemEditorIDs::Modes::EquipmentMode);
	}

	RegenerateMenusAndToolbars();
}

FText FItemEditorApp::GetToolkitName() const
{
	const UObject* EditingObj = GetCurrentMode() == ItemEditorIDs::Modes::ItemMode ? (UObject*)ItemDefinition : (UObject*)EquipmentDefinition;
	if (EditingObj)
	{
		return GetLabelForObject(EditingObj);
	}

	return FText::FromString("Invalid Object!");
}

void FItemEditorApp::OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit)
{
	FWorkflowCentricApplication::OnToolkitHostingStarted(Toolkit);
}

void FItemEditorApp::OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit)
{
	FWorkflowCentricApplication::OnToolkitHostingFinished(Toolkit);
}

void FItemEditorApp::SetCurrentMode(FName NewMode)
{
	FWorkflowCentricApplication::SetCurrentMode(NewMode);

	if (DetailsView.IsValid())
	{
		if (NewMode == ItemEditorIDs::Modes::ItemMode)
		{
			DetailsView->SetObject(ItemDefinition);
		}
		else if (NewMode == ItemEditorIDs::Modes::EquipmentMode)
		{
			DetailsView->SetObject(EquipmentDefinition);
		}
	}

	if (FragmentDetailsView.IsValid())
	{
		if (NewMode == ItemEditorIDs::Modes::ItemMode)
		{
			FragmentDetailsView->SetObject(ItemDefinition);
		}
		else if (NewMode == ItemEditorIDs::Modes::EquipmentMode)
		{
			FragmentDetailsView->SetObject(EquipmentDefinition);
		}
	}
}

FText FItemEditorApp::GetToolkitToolTipText() const
{
	return FText::FromString("Tooltip");
}

void FItemEditorApp::SaveAsset_Execute()
{
	FWorkflowCentricApplication::SaveAsset_Execute();

	// Update all pickup proxies
	UWorld* World = GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>()->GetEditorWorld();
	if (!World)
	{
		return;
	}
	
	for (TActorIterator<APickupProxyActor> It(World); It; ++It)
	{
		APickupProxyActor* PickupProxy = *It;
		if (PickupProxy->ItemDefinition.Get() == GetItemDefinition())
		{
			PickupProxy->OnConstruction(PickupProxy->GetActorTransform());
		}
	}
}

bool FItemEditorApp::CanSwitchToItemMode() const
{
	return GetItemDefinition() != nullptr;
}

bool FItemEditorApp::CanSwitchToEquipmentMode() const
{
	return GetEquipmentDefinition() != nullptr;
}

TSharedPtr<IDetailsView> FItemEditorApp::GetDetailsView() const
{
	return DetailsView;
}

TSharedPtr<IDetailsView> FItemEditorApp::GetFragmentDetailsView() const
{
	return FragmentDetailsView;
}

class UGameplayEquipmentDefinition* FItemEditorApp::GetEquipmentDefinition() const
{
	
	return ItemDefinition == nullptr ? EquipmentDefinition : ToRawPtr(ItemDefinition->EquipmentDefinition);
}
