// Copyright © 2024 MajorT. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class GAMEPLAYSETSEDITOR_API SPlaysetSelectedAssetDetails : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPlaysetSelectedAssetDetails)
		: _ShowThumbnail(EVisibility::Visible)
		, _MaxDesiredDescriptionWidth(300.f)
		{
		}
		SLATE_ATTRIBUTE(EVisibility, ShowThumbnail)
		SLATE_ARGUMENT(TOptional<FText>, OnAssetTagActivatedTooltip)
	
		/* Text will automatically wrap according to actual width.
		* With this you can specify the maximum width you want the description to be; this is not the actual width.
		* Actual width might be increased due to big asset names.
		*/
		SLATE_ARGUMENT(float, MaxDesiredDescriptionWidth)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const FAssetData& Asset);
	
private:
	FAssetData AssetData;
	TAttribute<EVisibility> ShowThumbnail;
	TSharedPtr<FAssetThumbnail> CurrentAssetThumbnail;
	TOptional<FText> OnAssetTagActivatedTooltip;

	TSharedRef<SWidget> CreateAssetThumbnailWidget();
	TSharedRef<SWidget> CreateTitleWidget();
	TSharedRef<SWidget> CreateTypeWidget();
	TSharedRef<SWidget> CreateDescriptionWidget();
	TSharedRef<SWidget> CreateOptionalPropertiesList();
};
