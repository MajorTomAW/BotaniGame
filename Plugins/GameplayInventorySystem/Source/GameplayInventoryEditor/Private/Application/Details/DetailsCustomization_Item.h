// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once
#include "IDetailCustomization.h"


class UGameplayInventoryItemDefinition;

class FDetailsCustomization_Item : public IDetailCustomization
{
public:
	FDetailsCustomization_Item(bool bShowFragmentDetails);
	
	static TSharedRef<IDetailCustomization> MakeInstance(bool bShowFragmentDetails = false);
	
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	bool bShowFragmentDetails = false;
};
