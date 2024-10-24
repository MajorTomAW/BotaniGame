// Copyright © 2024 MajorT. All rights reserved.

#include "SPlaysetInfoPanel.h"

#include "SlateOptMacros.h"
#include "Editor/PropertyEditor/Private/SDetailsView.h"

#define LOCTEXT_NAMESPACE "SPlaysetInfoPanel"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPlaysetInfoPanel::Construct(const FArguments& InArgs)
{
	PlaysetInfoObject = NewObject<UPlaysetInfoObject>();
	check(PlaysetInfoObject);
	
	// ConvertOptions = InArgs._ConvertOptions;
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.bShowOptions = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

	PlaysetDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	PlaysetDetailsView->SetObject(PlaysetInfoObject);
	
	ChildSlot
	[
		// Show the details view
		PlaysetDetailsView.ToSharedRef()
	];
}

void SPlaysetInfoPanel::GetPlaysetInfo(FPlaysetDisplayInfo& OutDisplayInfo, FPlaysetDataList& OutData, EPlaysetOffsetType& OffsetType) const
{
	OutDisplayInfo = PlaysetInfoObject->PlaysetDisplayInfo;
	OutData = PlaysetInfoObject->PlaysetData;
	OffsetType = PlaysetInfoObject->OffsetType;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE