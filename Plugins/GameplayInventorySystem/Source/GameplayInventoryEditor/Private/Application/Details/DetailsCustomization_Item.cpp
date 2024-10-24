// Copyright © 2024 Botanibots Team. All rights reserved.


#include "DetailsCustomization_Item.h"

#include "DetailLayoutBuilder.h"

FDetailsCustomization_Item::FDetailsCustomization_Item(bool bShowFragmentDetails)
	: bShowFragmentDetails(bShowFragmentDetails)
{
}

TSharedRef<IDetailCustomization> FDetailsCustomization_Item::MakeInstance(bool bShowFragmentDetails)
{
	return MakeShareable(new FDetailsCustomization_Item(bShowFragmentDetails));
}

void FDetailsCustomization_Item::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<FString> FragmentCategories;
	FragmentCategories.Add(TEXT("Fragments"));
	FragmentCategories.Add(TEXT("ItemFragments"));
	

	TArray<FName> Categories;
	DetailBuilder.GetCategoryNames(Categories);
	for (const auto& Category : Categories)
	{
		if (FragmentCategories.Contains(Category.ToString()))
		{
			if (!bShowFragmentDetails)
			{
				DetailBuilder.HideCategory(Category);
			}
		}
		else
		{
			if (bShowFragmentDetails)
			{
				DetailBuilder.HideCategory(Category);
			}
		}
	}
}
