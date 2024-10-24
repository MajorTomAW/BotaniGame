// Copyright © 2024 MajorT. All rights reserved.

#include "SPlaysetConvertAssetWindow.h"

#include "SlateOptMacros.h"
#include "SPlaysetSelectedActorsList.h"

#define LOCTEXT_NAMESPACE "SPlaysetConvertAssetWindow"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPlaysetConvertAssetWindow::Construct(const FArguments& InArgs)
{
	SelectedActorsList = InArgs._SelectedActorsList;
	
	FArguments Args = InArgs;
	TSharedRef<SWidget> ConvertAssetControls = SNew(SBox)
		.Padding(16.f)
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5.f, 1.f)
					[
						SNew(SButton)
						.ContentPadding(FMargin(50.f, 4.f, 50.f, 4.f))
						.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("PrimaryButton"))
						.TextStyle(&FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("PrimaryButtonText"))
						.HAlign(HAlign_Center)
						.Text(LOCTEXT("CreatePrimaryButtonLabel", "Save"))
						.OnClicked(this, &SPlaysetConvertAssetWindow::Proceed)
						.IsEnabled(this, &SPlaysetConvertAssetWindow::CanCreatePlayset)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5.f, 1.f)
					[
						SNew(SButton)
						.ContentPadding(FMargin(50.f, 4.f, 50.f, 4.f))
						.TextStyle(&FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("PrimaryButtonText"))
						.HAlign(HAlign_Center)
						.Text(LOCTEXT("CancelButtonLabel", "Cancel"))
						.OnClicked(this, &SPlaysetConvertAssetWindow::Cancel)
					]
				]
		];

	Args._PlaysetConverterArgs._PlaysetConverterArgs._AdditionalWidget = ConvertAssetControls;
	Args._PlaysetConverterArgs._PlaysetConverterArgs._OnAssetsActivated = FOnAssetsActivated::CreateSP(this, &SPlaysetConvertAssetWindow::OnAssetsActivatedInternal);
	SPlaysetConverterWindow::Construct(Args._PlaysetConverterArgs);
}

bool SPlaysetConvertAssetWindow::HasSelectedAssets() const
{
	return PlaysetConverter->GetActiveAssets().Num() > 0;
}

bool SPlaysetConvertAssetWindow::HasValidPlaysetName() const
{
	FPlaysetDisplayInfo DisplayInfo;
	FPlaysetDataList Data;
	EPlaysetOffsetType OffsetType;
	PlaysetConverter->GetPlaysetInfo(DisplayInfo, Data, OffsetType);

	return !DisplayInfo.ItemName.IsEmpty();
}

void SPlaysetConvertAssetWindow::OnAssetsActivatedInternal(const TArray<FAssetData>& AssetData, EAssetTypeActivationMethod::Type Method)
{
	bProceedWithAction = true;
}

FReply SPlaysetConvertAssetWindow::Proceed()
{
	OnAssetsActivated({}, EAssetTypeActivationMethod::Opened);
	return FReply::Handled();
}

FReply SPlaysetConvertAssetWindow::Cancel()
{
	bProceedWithAction = false;
	RequestDestroyWindow();
	return FReply::Handled();
}

bool SPlaysetConvertAssetWindow::CanCreatePlayset() const
{
	return HasSelectedAssets(); // && HasValidPlaysetName();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE