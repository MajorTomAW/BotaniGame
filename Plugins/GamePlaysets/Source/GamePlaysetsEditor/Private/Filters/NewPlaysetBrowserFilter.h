// Copyright © 2024 MajorT. All rights reserved.

#pragma once
#include "Playset.h"

struct FNewPlaysetBrowserFilter
{
	FNewPlaysetBrowserFilter()
	{
	}
	
public:
	TArray<TSubclassOf<UPlayset>> FilterClasses;
	FText CustomDisplayName;

	TArray<TSharedRef<FNewPlaysetBrowserFilter>> ChildFilters;

	FText GetDisplayName() const;
	FName GetIdentifier() const;

	FGuid CustomGuid;

public:
	bool operator==(const FNewPlaysetBrowserFilter& Other) const
	{
		return FilterClasses == Other.FilterClasses;
	}

	bool operator!=(const FNewPlaysetBrowserFilter& Other) const
	{
		return !(*this == Other);
	}
};
