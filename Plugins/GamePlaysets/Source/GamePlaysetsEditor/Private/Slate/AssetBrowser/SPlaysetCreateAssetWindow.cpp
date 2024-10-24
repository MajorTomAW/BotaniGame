// Copyright © 2024 MajorT. All rights reserved.

#include "SPlaysetCreateAssetWindow.h"

#include "SlateOptMacros.h"
#include "Styling/SlateIconFinder.h"

#define LOCTEXT_NAMESPACE "SPlaysetCreateAssetWindow"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPlaysetCreateAssetWindow::Construct(const FArguments& InArgs, UClass& InCreatedClass)
{
	CreatedClass = &InCreatedClass;

	FArguments Args = InArgs;

	TSharedRef<SWidget> CreateAssetControls = SNew(SBox)
		.Padding(16.f, 16.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.f, 1.f)
			.HAlign(HAlign_Left)
			[
				SNew(SButton)
				.OnClicked(this, &SPlaysetCreateAssetWindow::ProceedTemplate)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2.f)
					[
						SNew(SImage)
						.Image(FSlateIconFinder::FindIconForClass(CreatedClass.Get()).GetIcon())
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2.f)
					[
						SNew(STextBlock).Text(FText::FormatOrdered(LOCTEXT("CreateEmptyAssetButtonLabel", "Create Template {0}"), CreatedClass->GetDisplayNameText()))
					]
				]
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SSpacer)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.f, 1.f)
			[
				SNew(SButton)
				.ContentPadding(FMargin(50.f, 4.f, 50.f, 1.f))
				.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("PrimaryButton"))
				.TextStyle(&FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("PrimaryButtonText"))
				.HAlign(HAlign_Center)
				.Text(LOCTEXT("CreatePrimaryButtonLabel", "Create"))
				// .Text(FText::FormatOrdered(LOCTEXT("CreatePrimaryButtonLabel", "Create {0}"), CreatedClass->GetDisplayNameText()))
				.OnClicked(this, &SPlaysetCreateAssetWindow::Proceed)
				.IsEnabled(this, &SPlaysetCreateAssetWindow::HasSelectedAssets)
				.ToolTipText(this, &SPlaysetCreateAssetWindow::GetCreateButtonTooltip)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.f, 1.f)
			[
				SNew(SButton)
				.ContentPadding(FMargin(50.f, 4.f, 50.f, 1.f))
				.TextStyle(&FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("PrimaryButtonText"))
				.HAlign(HAlign_Center)
				.Text(LOCTEXT("CancelButtonLabel", "Cancel"))
				.OnClicked(this, &SPlaysetCreateAssetWindow::Cancel)
			]
		];
	
	Args._AssetBrowserWindowArgs._AssetBrowserArgs._AdditionalWidget = CreateAssetControls;
	Args._AssetBrowserWindowArgs._AssetBrowserArgs._OnAssetsActivated = FOnAssetsActivated::CreateSP(this, &SPlaysetCreateAssetWindow::OnAssetsActivatedInternal);
	SPlaysetAssetBrowserWindow::Construct(Args._AssetBrowserWindowArgs);
}

void SPlaysetCreateAssetWindow::OnAssetsActivatedInternal(const TArray<FAssetData>& AssetData, EAssetTypeActivationMethod::Type Method)
{
	bProceedWithAction = true;
}

FReply SPlaysetCreateAssetWindow::Proceed()
{
	// The assets don't matter here as the factories making use of this window will retrieve the selected assets afterwards
	OnAssetsActivated({}, EAssetTypeActivationMethod::Opened, false);
	return FReply::Handled();
}

FReply SPlaysetCreateAssetWindow::ProceedTemplate()
{
	// The assets don't matter here as the factories making use of this window will retrieve the selected assets afterwards
	OnAssetsActivated({}, EAssetTypeActivationMethod::Opened, true);
	return FReply::Handled();
}

FReply SPlaysetCreateAssetWindow::Cancel()
{
	bProceedWithAction = false;
	RequestDestroyWindow();
	return FReply::Handled();
}

FText SPlaysetCreateAssetWindow::GetCreateButtonTooltip() const
{
	return HasSelectedAssets()
	? FText::FormatOrdered(LOCTEXT("CreateAssetButtonTooltip_Enabled", "Create a new {0} with selected asset {1}"), CreatedClass->GetDisplayNameText(), FText::FromName(GetSelectedAssets()[0].AssetName))
	: LOCTEXT("CreateAssetButtonTooltip_Disabled", "Please select a Playset Item as template. Alternatively, create an empty Playset.");
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE