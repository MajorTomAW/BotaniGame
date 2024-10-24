// Copyright © 2024 Botanibots Team. All rights reserved.


#include "itemDefinitionDetails.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "Definitions/GameplayInventoryItemDefinition.h"

#define LOCTEXT_NAMESPACE "FItemDefinitionDetails"

TSharedRef<IDetailCustomization> FItemDefinitionDetails::MakeInstance()
{
	return MakeShared<FItemDefinitionDetails>();
}

void FItemDefinitionDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& PlaysetCategory = DetailBuilder.EditCategory("Playset");
	DetailBuilder.EditCategory("Playset|Placement").SetCategoryVisibility(false);
	
	PlaysetCategory.SetDisplayName(LOCTEXT("PlaysetCategory", "Display"));
}


#undef LOCTEXT_NAMESPACE