// Copyright © 2024 Botanibots Team. All rights reserved.


#include "TabFactory_ItemDetails.h"

#include "GameplayInventoryEditor.h"
#include "Application/ItemEditorApp.h"

#define LOCTEXT_NAMESPACE "GameplayInventoryEditor"

FTabFactory_ItemDetails::FTabFactory_ItemDetails(const TSharedPtr<class FItemEditorApp>& App)
	: FWorkflowTabFactory(ItemEditorIDs::Tabs::ItemTab_Details, App)
	, AppPtr(App)
{
	TabLabel = LOCTEXT("ItemDetailsTabLabel", "Item Details");
	TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details");
	TabRole = ETabRole::PanelTab;

	ViewMenuDescription = LOCTEXT("ItemDetailsView", "Item Details");
	ViewMenuTooltip = LOCTEXT("ItemDetailsView_Tooltip", "Show the Item Details view");
}

TSharedRef<SWidget> FTabFactory_ItemDetails::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<FItemEditorApp> App = AppPtr.Pin();
	check(App);

	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		.HAlign(HAlign_Fill)
		[
			App->GetDetailsView().ToSharedRef()
		];
}

#undef LOCTEXT_NAMESPACE