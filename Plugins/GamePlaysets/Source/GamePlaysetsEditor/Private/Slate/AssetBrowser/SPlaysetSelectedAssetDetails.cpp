// Copyright © 2024 MajorT. All rights reserved.

#include "SPlaysetSelectedAssetDetails.h"

#include "Playset.h"
#include "SlateOptMacros.h"
#include "Styling/SlateIconFinder.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "Widgets/Layout/SScaleBox.h"

#define LOCTEXT_NAMESPACE "SPlaysetSelectedAssetDetails"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION


void SPlaysetSelectedAssetDetails::Construct(const FArguments& InArgs, const FAssetData& Asset)
{
	AssetData = Asset;
	if (!AssetData.IsValid())
	{
		return;
	}

	ShowThumbnail = InArgs._ShowThumbnail;
	OnAssetTagActivatedTooltip = InArgs._OnAssetTagActivatedTooltip;

	ChildSlot
	[
		SNew(SBox)
		.MinDesiredWidth(228.f)
		.MaxDesiredWidth(228.f)
		.Padding(11.5f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Padding(0.f, 0.f, 0.f, 22.f)
			[
				CreateAssetThumbnailWidget()
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f)
			[
				CreateTitleWidget()
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Padding(0.f, 6.f)
			[
				SNew(SScrollBox)
				+ SScrollBox::Slot()
				[
					SNew(SBorder)
					.BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 1.f))
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.Padding(8.f)
						[
							SNew(STextBlock)
							.TextStyle(&FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("PrimaryButtonText"))
							.Font(FSlateFontInfo(FAppStyle::Get().GetFontStyle("NotificationList.FontBold")))
							.Text(NSLOCTEXT("DescriptionText", "PlaysetDescription", "Description"))
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(8.f, 16.f, 8.f, 8.f)
						[
							SNew(SBox)
							.MaxDesiredWidth(InArgs._MaxDesiredDescriptionWidth)
							[
								CreateDescriptionWidget()
							]
						]
					]
				]
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 6.f)
			[
				CreateOptionalPropertiesList()
			]
		]
	];
}

TSharedRef<SWidget> SPlaysetSelectedAssetDetails::CreateAssetThumbnailWidget()
{
	CurrentAssetThumbnail = MakeShared<FAssetThumbnail>(AssetData, 256.f, 256.f, UThumbnailManager::Get().GetSharedThumbnailPool());
	FAssetThumbnailConfig Config;
	Config.bAllowRealTimeOnHovered = false;
	
	return SNew(SBox)
	.WidthOverride(192.f)
	.HeightOverride(192.f)
	.Visibility(ShowThumbnail)
	[
		SNew(SScaleBox)
		.Stretch(EStretch::ScaleToFill)
		[
			CurrentAssetThumbnail->MakeThumbnailWidget(Config)
		]
	];
}

TSharedRef<SWidget> SPlaysetSelectedAssetDetails::CreateTitleWidget()
{
	FText AssetName = FText::FromName(AssetData.AssetName);
	
	UObject* Asset = AssetData.GetAsset();
	const UPlayset* Playset = Cast<UPlayset>(Asset);
	
	if (Playset && !Playset->DisplayInfo.ItemName.IsEmpty())
	{
		AssetName = Playset->DisplayInfo.ItemName;
	}
	
	return SNew(SVerticalBox)
	+ SVerticalBox::Slot()
	.AutoHeight()
	.HAlign(HAlign_Left)
	.Padding(2.f)
	[
		SNew(STextBlock)
		.Text(AssetName)
		.TextStyle(&FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("PrimaryButtonText"))
		.Font(FSlateFontInfo(FAppStyle::Get().GetFontStyle("NotificationList.FontBold")))
	]
	+ SVerticalBox::Slot()
	.AutoHeight()
	.HAlign(HAlign_Left)
	.Padding(2.f)
	[
		CreateTypeWidget()
	];
}

TSharedRef<SWidget> SPlaysetSelectedAssetDetails::CreateTypeWidget()
{
	return SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.AutoWidth()
	.Padding(2.f)
	[
		SNew(SImage)
		.Image(FSlateIconFinder::FindIconForClass(AssetData.GetClass()).GetIcon())
	]
	+ SHorizontalBox::Slot()
	.AutoWidth()
	[
		SNew(STextBlock)
		.Text(AssetData.GetClass()->GetDisplayNameText())
		.TextStyle(FAppStyle::Get(), "ButtonText")
	];
}

TSharedRef<SWidget> SPlaysetSelectedAssetDetails::CreateDescriptionWidget()
{
	FText DescriptionText = FText::FromString("No description available.");
	
	UObject* Asset = AssetData.GetAsset();
	const UPlayset* Playset = Cast<UPlayset>(Asset);
	
	if (Playset && !Playset->DisplayInfo.ItemDescription.IsEmpty())
	{
		DescriptionText = Playset->GetPlaysetDesc();
	}
	
	return SNew(STextBlock)
	.AutoWrapText(true)
	.Text(DescriptionText)
	.Visibility(EVisibility::Visible);
}

TSharedRef<SWidget> SPlaysetSelectedAssetDetails::CreateOptionalPropertiesList()
{
	return SNew(SBox).Visibility(EVisibility::Collapsed);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE