// Copyright © 2024 MajorT. All rights reserved.

#include "PlaysetMenuFilters.h"

#include "Playset.h"

FName FPlaysetAssetBrowserMainFilter::GetFilter(const FAssetData& InAssetData)
{
	UPlayset* Playset = GetPlaysetFromAssetData(InAssetData);
	if (Playset)
	{
		//@TODO: Get the filter mode from the Playset
	}

	return NAME_None;
}

FText FPlaysetAssetBrowserMainFilter::GetDisplayName() const
{
	return FText::FromName(FilterMode);
}

UPlayset* FPlaysetAssetBrowserMainFilter::GetPlaysetFromAssetData(const FAssetData& InAssetData)
{
	return Cast<UPlayset>(InAssetData.GetAsset());
}
