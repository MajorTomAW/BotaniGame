// Copyright © 2024 Botanibots Team. All rights reserved.


#include "ApplicationMode_Equipment.h"

#include "GameplayInventoryEditor.h"
#include "Application/ItemEditorApp.h"
#include "Application/Tabs/TabFactory_FragmentDetails.h"
#include "Application/Tabs/TabFactory_ItemDetails.h"
#include "Application/Toolbar/ItemEditorToolbar.h"

FApplicationMode_Equipment::FApplicationMode_Equipment(TSharedPtr<FItemEditorApp> App)
	: FApplicationMode(ItemEditorIDs::Modes::EquipmentMode)
	, AppPtr(App)
{
	TabFactories.RegisterFactory(MakeShareable(new FTabFactory_ItemDetails(App)));
	TabFactories.RegisterFactory(MakeShareable(new FTabFactory_FragmentDetails(App)));

	TabLayout = FTabManager::NewLayout("Standalone_EquipmentEditor_Layout_v1")
	->AddArea
	(
		FTabManager::NewPrimaryArea()
		->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewSplitter()
			->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.6f)
				->AddTab(ItemEditorIDs::Tabs::ItemTab_Details, ETabState::OpenedTab)
			)

			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.4f)
				->AddTab(ItemEditorIDs::Tabs::ItemTab_Fragments, ETabState::OpenedTab)
			)
		)
	);
	
	App->GetToolbarBuilder()->AddModesToolbar(ToolbarExtender);
}

void FApplicationMode_Equipment::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	check(AppPtr.IsValid());
	TSharedPtr<FItemEditorApp> App = AppPtr.Pin();
	App->PushTabFactories(TabFactories);
	
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FApplicationMode_Equipment::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void FApplicationMode_Equipment::PostActivateMode()
{
	FApplicationMode::PostActivateMode();
}
