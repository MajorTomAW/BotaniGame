// Copyright © 2024 MajorT. All rights reserved.
#pragma once

class UPlayset;

struct FPlaysetAssetBrowserMainFilter
{
public:
	static FName GetFilter(const FAssetData& InAssetData);
	FText GetDisplayName() const;

	FName FilterMode;

	FPlaysetAssetBrowserMainFilter()
		: FilterMode(NAME_None)
	{
	}

	FPlaysetAssetBrowserMainFilter(FName InFilterMode)
		: FilterMode(InFilterMode)
	{
	}

	bool operator==(const FPlaysetAssetBrowserMainFilter& Other) const
	{
		return FilterMode == Other.FilterMode;
	}

private:
	static UPlayset* GetPlaysetFromAssetData(const FAssetData& InAssetData);
};