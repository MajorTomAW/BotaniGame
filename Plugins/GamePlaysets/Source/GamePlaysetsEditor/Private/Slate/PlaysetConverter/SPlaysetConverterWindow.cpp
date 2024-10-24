// Copyright © 2024 MajorT. All rights reserved.

#include "SPlaysetConverterWindow.h"

#include "SPlaysetConverter.h"
#include "SlateOptMacros.h"

#define LOCTEXT_NAMESPACE "SPlaysetConverterWindow"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPlaysetConverterWindow::Construct(const FArguments& InArgs)
{
	OnAssetsActivatedDelegate = InArgs._PlaysetConverterArgs._OnAssetsActivated;
	
	SWindow::Construct(SWindow::FArguments()
		.Title(InArgs._WindowTitle.Get(LOCTEXT("PlaysetConverterWindowTitle", "Playset Converter")))
		.SizingRule(ESizingRule::UserSized)
		.ClientSize(FVector2D(800.f, 720.f))
		.SupportsMaximize(true)
		.SupportsMinimize(false)
	[
		SAssignNew(PlaysetConverter, SPlaysetConverter)
		.SelectedActors(InArgs._PlaysetConverterArgs._SelectedActors)
		.AdditionalWidget(InArgs._PlaysetConverterArgs._AdditionalWidget)
	]);
}

TArray<FAssetData> SPlaysetConverterWindow::GetActiveAssets() const
{
	return PlaysetConverter->GetActiveAssets();
}

void SPlaysetConverterWindow::GetPlaysetInfo(FPlaysetDisplayInfo& OutDisplayInfo, FPlaysetDataList& OutData, EPlaysetOffsetType& OffsetType) const
{
	return PlaysetConverter->GetPlaysetInfo(OutDisplayInfo, OutData, OffsetType);
}

/*const FPlaysetInfoObjectStruct& SPlaysetConverterWindow::GetPlaysetInfo() const
{
	return PlaysetConverter->GetPlaysetInfo();
}*/

/*FPlaysetConverterOptions SPlaysetConverterWindow::GetPlaysetOptions() const
{
	FPlaysetConverterOptions Options;
	return Options;
}*/

void SPlaysetConverterWindow::OnAssetsActivated(const TArray<FAssetData>& AssetData, EAssetTypeActivationMethod::Type ActivationType)
{
	if (OnAssetsActivatedDelegate.ExecuteIfBound(AssetData, ActivationType))
	{
		RequestDestroyWindow();
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE