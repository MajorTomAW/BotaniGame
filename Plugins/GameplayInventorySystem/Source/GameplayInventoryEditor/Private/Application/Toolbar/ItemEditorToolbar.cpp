// Copyright © 2024 Botanibots Team. All rights reserved.


#include "ItemEditorToolbar.h"

#include "GameplayInventoryEditor.h"
#include "Application/ItemEditorApp.h"
#include "WorkflowOrientedApp/SModeWidget.h"

#define LOCTEXT_NAMESPACE "ItemEditorToolbar"

void FItemEditorToolbar::AddModesToolbar(TSharedPtr<FExtender> Extender)
{
	check(AppPtr.IsValid());
	TSharedPtr<FItemEditorApp> App = AppPtr.Pin();

	Extender->AddToolBarExtension
	(
		"Asset",
		EExtensionHook::After,
		App->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FItemEditorToolbar::FillModesToolbar)
	);
}

void FItemEditorToolbar::AddEditorToolbar(TSharedPtr<FExtender> Extender)
{

}

void FItemEditorToolbar::FillModesToolbar(FToolBarBuilder& ToolbarBuilder)
{
	check(AppPtr.IsValid());
	TSharedPtr<FItemEditorApp> App = AppPtr.Pin();

	TAttribute<FName> GetActiveMode(App.ToSharedRef(), &FItemEditorApp::GetCurrentMode);
	FOnModeChangeRequested SetActiveMode = FOnModeChangeRequested::CreateSP(App.ToSharedRef(), &FItemEditorApp::SetCurrentMode);

	const FSlateBrush* ItemBrush = FAppStyle::GetBrush("ClassIcon.DataAsset");
	const FSlateBrush* EquipmentBrush = FAppStyle::GetBrush("ClassIcon.Object");

	FSlateBrush* MutableItemBrush = const_cast<FSlateBrush*>(ItemBrush);
	FSlateBrush* MutableEquipmentBrush = const_cast<FSlateBrush*>(EquipmentBrush);
	constexpr float IconSize = 22.f;
	MutableItemBrush->ImageSize = FVector2D(IconSize);
	MutableEquipmentBrush->ImageSize = FVector2D(IconSize);

	//MutableItemBrush.SetImageSize(FVector2d(42.f, 42.f));
	//MutableEquipmentBrush.SetImageSize(FVector2d(42.f, 42.f));

	// Left side padding
	App->AddToolbarWidget(SNew(SSpacer).Size(FVector2d(4.f, 1.f)));

	App->AddToolbarWidget
	(
		SNew(SModeWidget, LOCTEXT("ItemMode", "Item"), ItemEditorIDs::Modes::ItemMode)
		.OnGetActiveMode(GetActiveMode)
		.OnSetActiveMode(SetActiveMode)
		.CanBeSelected(App.Get(), &FItemEditorApp::CanSwitchToItemMode)
		.ToolTipText(LOCTEXT("ItemMode_Tooltip", "Switch to Item Mode"))
		.IconImage(MutableItemBrush)
	);

	App->AddToolbarWidget(SNew(SSpacer).Size(FVector2d(10.f, 1.f)));

	App->AddToolbarWidget
	(
		SNew(SModeWidget, LOCTEXT("EquipmentMode", "Equipment"), ItemEditorIDs::Modes::EquipmentMode)
		.OnGetActiveMode(GetActiveMode)
		.OnSetActiveMode(SetActiveMode)
		.CanBeSelected(App.Get(), &FItemEditorApp::CanSwitchToEquipmentMode)
		.ToolTipText(LOCTEXT("EquipmentMode_Tooltip", "Switch to Equipment Mode"))
		.IconImage(EquipmentBrush)
	);


	// Right side padding
	App->AddToolbarWidget(SNew(SSpacer).Size(FVector2d(10.f, 1.f)));
}

#undef LOCTEXT_NAMESPACE
