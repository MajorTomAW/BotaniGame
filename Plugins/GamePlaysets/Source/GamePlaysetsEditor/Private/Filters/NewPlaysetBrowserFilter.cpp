// Copyright © 2024 MajorT. All rights reserved.


#include "NewPlaysetBrowserFilter.h"

FText FNewPlaysetBrowserFilter::GetDisplayName() const
{
	if (CustomDisplayName.IsEmpty())
	{
		return FilterClasses.Top() ? FText::FromString(FilterClasses.Top()->GetName()) : FText::GetEmpty();
	}

	return CustomDisplayName;
}

FName FNewPlaysetBrowserFilter::GetIdentifier() const
{
	return FName(FText::AsCultureInvariant(GetDisplayName()).ToString());
}
