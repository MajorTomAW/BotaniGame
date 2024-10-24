// Copyright © 2024 Botanibots Team. All rights reserved.


#include "TabFactory_FragmentDetails.h"

#include "GameplayInventoryEditor.h"
#include "Application/ItemEditorApp.h"

#define LOCTEXT_NAMESPACE "GameplayInventoryEditor"

FTabFactory_FragmentDetails::FTabFactory_FragmentDetails(const TSharedPtr<class FItemEditorApp>& App)
	: FWorkflowTabFactory(ItemEditorIDs::Tabs::ItemTab_Fragments, App)
	, AppPtr(App)
{
	TabLabel = LOCTEXT("ItemDetailsTabLabel", "Fragment Details");
	TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details");
	TabRole = ETabRole::PanelTab;

	ViewMenuDescription = LOCTEXT("ItemDetailsView", "Fragment Details");
	ViewMenuTooltip = LOCTEXT("ItemDetailsView_Tooltip", "Show the Item Details view");
}

TSharedRef<SWidget> FTabFactory_FragmentDetails::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<FItemEditorApp> App = AppPtr.Pin();
	check(App);

	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		.HAlign(HAlign_Fill)
		[
			App->GetFragmentDetailsView().ToSharedRef()
		];
}

#undef LOCTEXT_NAMESPACE