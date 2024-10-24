// Copyright © 2024 MajorT. All rights reserved.

#include "PlaysetDetailsCustomization.h"

TSharedRef<IDetailCustomization> FPlaysetDetailsCustomization::MakeInstance()
{
	return MakeShared<FPlaysetDetailsCustomization>();
}

void FPlaysetDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
}


