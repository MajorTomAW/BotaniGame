// Copyright © 2024 MajorT. All rights reserved.

#include "SPlaysetSelectedActorsList.h"

#include "SelectedPlaysetActor.h"
#include "SlateOptMacros.h"
#include "Styling/SlateIconFinder.h"

#define LOCTEXT_NAMESPACE "SPlaysetSelectedActorsList"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPlaysetSelectedActorsList::Construct(const FArguments& InArgs)
{
	SelectedWeakActors = InArgs._SelectedActors;
	
	SelectedActors = GetSelectedActors();
	SelectedActiveActors = SelectedActors;

	SelectedAssetsText = SNew(STextBlock)
			.Text(GetSelectedActorsText())
			.TextStyle(&FAppStyle::GetWidgetStyle<FTextBlockStyle>("NormalText.Subdued"));

	SelectionTileView = SNew(STileView<TSharedRef<FSelectedPlaysetActor>>)
				.ListItemsSource(&SelectedActors)
				.Orientation(Orient_Vertical)
				.ItemHeight(32.f)
				.ItemWidth(300.f)
				.ItemAlignment(EListItemAlignment::Fill)
				.SelectionMode(ESelectionMode::Type::Multi)
				.OnGenerateTile(this, &SPlaysetSelectedActorsList::GenerateTile);
	
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(8.f, 16.f, 8.f, 8.f)
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			SelectedAssetsText.ToSharedRef()
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SBorder)
			.Padding(0.f)
			.BorderImage(FAppStyle::Get().GetBrush("Brushes.Recessed"))
			[
				SelectionTileView.ToSharedRef()
			]
		]
	];
}

TArray<FAssetData> SPlaysetSelectedActorsList::GetSelectedAssets() const
{
	TArray<FAssetData> SelectedAssets;
	for (const auto& Selected : SelectionTileView->GetSelectedItems())
	{
		SelectedAssets.Add(Selected->GetAssetData());
	}

	return SelectedAssets;
}

TArray<FAssetData> SPlaysetSelectedActorsList::GetActiveAssets() const
{
	TArray<FAssetData> ActiveAssets;
	for (const auto& Selected : SelectedActiveActors)
	{
		ActiveAssets.Add(Selected->GetAssetData());
	}

	return ActiveAssets;
}

TArray<TSharedRef<FSelectedPlaysetActor>> SPlaysetSelectedActorsList::GetSelectedActors() const
{
	TArray<TSharedRef<FSelectedPlaysetActor>> FoundSelectedActors;

	for (const TWeakObjectPtr<AActor>& WeakActor : SelectedWeakActors)
	{
		if (AActor* Actor = WeakActor.Get())
		{
			FoundSelectedActors.Add(MakeShared<FSelectedPlaysetActor>(Actor));
		}
	}


	return FoundSelectedActors;
}

TSharedRef<ITableRow> SPlaysetSelectedActorsList::GenerateTile(TSharedRef<FSelectedPlaysetActor> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	TSharedRef<SWidget> Widget = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		.Padding(8.f)
		[
			SNew(SCheckBox)
			.HAlign(HAlign_Center)
			.IsChecked(IsItemActive(InItem))
			.OnCheckStateChanged(this, &SPlaysetSelectedActorsList::OnCheckStateChanged, InItem)
		]
		+ SHorizontalBox::Slot()
		.Padding(4.f)
		.HAlign(HAlign_Left)
		.AutoWidth()
		[
			SNew(SImage)
			.DesiredSizeOverride(FVector2d(24.f))
			// .Image(FAppStyle::Get().GetBrush("Icons.Actor"))
			.Image(FSlateIconFinder::FindIconForClass(InItem->GetActorClass()).GetIcon())
		]
		
		+ SHorizontalBox::Slot()
		.Padding(16.f, 0.f, 16.f, 0.f)
		.AutoWidth()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(InItem->GetActorName()))
			.TextStyle(GetTextStyle(InItem))
			.ColorAndOpacity(GetTextColor(InItem))
		];

	return SNew(STableRow<TSharedRef<FSelectedPlaysetActor>>, OwnerTable)
		.ToolTip(CreateToolTip(InItem))
		[
			Widget
		];
}

FText SPlaysetSelectedActorsList::GetSelectedActorsText() const
{
	return FText::Format(LOCTEXT("SelectedActorsText", "Selected Actors: [{0}/{1}]"), FText::AsNumber(SelectedActiveActors.Num()), FText::AsNumber(SelectedActors.Num()));
}

void SPlaysetSelectedActorsList::OnCheckStateChanged(ECheckBoxState NewState, TSharedRef<FSelectedPlaysetActor> InItem)
{
	bool bIsChecked = NewState == ECheckBoxState::Checked;
	if (NewState == ECheckBoxState::Checked)
	{
		InItem.Get().bEnabled = bIsChecked;
		SelectedActiveActors.AddUnique(InItem);
	}
	else if (NewState == ECheckBoxState::Unchecked)
	{
		InItem.Get().bEnabled = bIsChecked;
		SelectedActiveActors.Remove(InItem);
	}
	
	// Updated selected items
	for (auto& Selected : SelectionTileView->GetSelectedItems())
	{
		if (bIsChecked)
		{
			// add to active actors list
			SelectedActiveActors.AddUnique(Selected);
		}
		else
		{
			// remove from active actors list
			SelectedActiveActors.Remove(Selected);	
		}
	}
	
	SelectedAssetsText->SetText(GetSelectedActorsText());
	SelectionTileView->RebuildList();
}

bool SPlaysetSelectedActorsList::IsItemActive(TSharedRef<FSelectedPlaysetActor> InItem) const
{
	// Item is active if its inside the selected active actors list
	return SelectedActiveActors.Contains(InItem);
}

const FTextBlockStyle* SPlaysetSelectedActorsList::GetTextStyle(TSharedRef<FSelectedPlaysetActor> InItem) const
{
	if (IsItemActive(InItem))
	{
		return &FAppStyle::GetWidgetStyle<FTextBlockStyle>("NormalText");
	}

	// Return subdued text style
	return &FAppStyle::GetWidgetStyle<FTextBlockStyle>("NormalText.Subdued");
}

const FSlateColor SPlaysetSelectedActorsList::GetTextColor(TSharedRef<FSelectedPlaysetActor> InItem) const
{
	if (IsItemActive(InItem))
	{
		return FSlateColor(FLinearColor::White);
	}

	// Return subdued text color
	return FSlateColor(FLinearColor::Red);
}


TSharedRef<class SToolTip> SPlaysetSelectedActorsList::CreateToolTip(const TSharedRef<FSelectedPlaysetActor>& InItem)
{
	TSharedRef<SToolTip> ToolTip = SNew(SToolTip)
		[
			SNew(SBox)
			.Padding(8.f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(InItem->GetActorName()))
			]
		];

	return ToolTip;
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE