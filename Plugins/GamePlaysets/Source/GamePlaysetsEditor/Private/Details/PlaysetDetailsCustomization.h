// Copyright © 2024 MajorT. All rights reserved.
#pragma once
#include "IDetailCustomization.h"

class FPlaysetDetailsCustomization : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	//~ Begin IPropertyTypeCustomization Interface
	virtual void CustomizeDetails( IDetailLayoutBuilder& DetailLayout ) override;
	//~ End IPropertyTypeCustomization Interface
};
