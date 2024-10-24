// Copyright © 2024 Botanibots Team. All rights reserved.


#include "SNewPlaysetPickerWindow.h"

#include "SlateOptMacros.h"
#include "SPrimaryButton.h"

#define LOCTEXT_NAMESPACE "SNewPlaysetPickerWindow"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SNewPlaysetPickerWindow::Construct(const FArguments& InArgs)
{
	FArguments Args = InArgs;

	SelectedClass = nullptr;
	
	TSharedRef<SWidget> AssetControls = SNew(SBox)
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Bottom)
		.Padding(16.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.f, 1.f)
			.HAlign(HAlign_Right)
			[
				SNew(SPrimaryButton)
				// On clicked
				//.ContentPadding(FMargin(5.f, 1.f))
				//.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("PrimaryButton"))
				//.TextStyle(&FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("PrimaryButtonText"))
				//.HAlign(HAlign_Center)
				.Visibility(this, &SNewPlaysetPickerWindow::GetSelectButtonVisibility)
				.Text(LOCTEXT("CreatePlaysetButton", "Select"))
				.OnClicked(FOnClicked::CreateSP(this, &SNewPlaysetPickerWindow::OnSelectButtonClicked))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.f, 1.f)
			[
				SNew(SButton)
				// On clicked
				//.ContentPadding(FMargin(5.f, 1.f))
				//.TextStyle(&FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("PrimaryButtonText"))
				.HAlign(HAlign_Center)
				.Text(LOCTEXT("CancelButtonLabel", "Cancel"))
				.OnClicked(FOnClicked::CreateSP(this, &SNewPlaysetPickerWindow::OnCancelButtonClicked))
			]
		];

	Args._NewPlaysetPickerArgs._AdditionalWidget = AssetControls;
	Args._NewPlaysetPickerArgs._OnClassPicked = FOnClassPicked::CreateSP(this, &SNewPlaysetPickerWindow::OnClassPicked);
	
	
	SWindow::Construct(SWindow::FArguments()
		.Title(Args._WindowTitle.Get(LOCTEXT("PlaysetPickerWindowTitle", "Select a Playset")))
		.SizingRule(ESizingRule::UserSized)
		.ClientSize(FVector2D(560.f, 720.f))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		[
			SAssignNew(NewPlaysetPicker, SNewPlaysetPicker)
			.AssetClass(Args._NewPlaysetPickerArgs._AssetClass)
			.AssetSelectionMode(Args._NewPlaysetPickerArgs._AssetSelectionMode)
			.OnAssetSelected(Args._NewPlaysetPickerArgs._OnAssetSelected)
			.AdditionalWidget(Args._NewPlaysetPickerArgs._AdditionalWidget)
			.OnClassPicked(Args._NewPlaysetPickerArgs._OnClassPicked)
		]);
}

EVisibility SNewPlaysetPickerWindow::GetSelectButtonVisibility() const
{
	if (SelectedClass != nullptr)
	{
		return EVisibility::Visible;
	}

	return EVisibility::Hidden;
}

FReply SNewPlaysetPickerWindow::OnSelectButtonClicked()
{
	bProceedWithAction = true;
	RequestDestroyWindow();
	return FReply::Handled();
}

FReply SNewPlaysetPickerWindow::OnCancelButtonClicked()
{
	bProceedWithAction = false;
	RequestDestroyWindow();
	return FReply::Handled();
}

void SNewPlaysetPickerWindow::OnClassPicked(UClass* InChosenClass)
{
	SelectedClass = InChosenClass;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE