// Copyright © 2024 MajorT. All rights reserved.

#include "SPlaysetConverter.h"

#include "SlateOptMacros.h"
#include "SPlaysetInfoPanel.h"
#include "SPlaysetSelectedActorsList.h"

#define LOCTEXT_NAMESPACE "SPlaysetConverter"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPlaysetConverter::Construct(const FArguments& InArgs)
{
	SelectedActors = InArgs._SelectedActors;
	
	ChildSlot
	[
		SNew(SBorder)
		.Padding(0.f)
		.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
		[
			SNew(SVerticalBox)

			// Content
			+ SVerticalBox::Slot()
			.Padding(0.f)
			[
				SNew(SSplitter)
				.Orientation(Orient_Horizontal)
				.PhysicalSplitterHandleSize(2.f)
				+ SSplitter::Slot()
				.Expose(SelectedActorsSlot)
				.Value(0.5)
				+ SSplitter::Slot()
				.Expose(PlaysetSlot)
				.Value(0.5)
			]

			// Additional widget
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SSeparator)
				.Orientation(Orient_Horizontal)
				.Thickness(2.f)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Expose(AdditionalWidgetSlot)
		]
	];

	
	PopulatePlaysetSlot();
	PopulateSelectedActorsSlot();

	if (InArgs._AdditionalWidget.IsValid())
	{
		AdditionalWidgetSlot->AttachWidget(InArgs._AdditionalWidget.ToSharedRef());
	}
}

TArray<FAssetData> SPlaysetConverter::GetSelectedAssets() const
{
	return SelectedActorsList->GetSelectedAssets();
}

TArray<FAssetData> SPlaysetConverter::GetActiveAssets() const
{
	return SelectedActorsList->GetActiveAssets();
}

void SPlaysetConverter::GetPlaysetInfo(FPlaysetDisplayInfo& OutDisplayInfo, FPlaysetDataList& OutData, EPlaysetOffsetType& OffsetType) const
{
	PlaysetInfoPanel->GetPlaysetInfo(OutDisplayInfo, OutData, OffsetType);
}

/*const FPlaysetInfoObjectStruct& SPlaysetConverter::GetPlaysetInfo() const
{
	return PlaysetInfoPanel->GetPlaysetInfo();
}*/

void SPlaysetConverter::PopulatePlaysetSlot()
{
	//const TSharedPtr<FPlaysetConverterOptions> ConvertOptions = MakeShared<FPlaysetConverterOptions>();

	PlaysetInfoPanel = SNew(SPlaysetInfoPanel);
		//.ConvertOptions(ConvertOptions.ToSharedRef());

	PlaysetSlot->AttachWidget(PlaysetInfoPanel.ToSharedRef());
}

void SPlaysetConverter::PopulateSelectedActorsSlot()
{
	SelectedActorsList = SNew(SPlaysetSelectedActorsList)
		.SelectedActors(SelectedActors);

	SelectedActorsSlot->AttachWidget(SelectedActorsList.ToSharedRef());
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE