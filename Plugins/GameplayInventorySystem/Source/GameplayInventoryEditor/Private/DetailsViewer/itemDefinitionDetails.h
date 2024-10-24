// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once
#include "IDetailCustomization.h"


class FItemDefinitionDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	//~ Begin IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	//~ End IDetailCustomization interface
};
