// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once
#include "IDetailCustomization.h"


class FBotaniEquipmentDetailsViewer : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

public:
	//~ Begin IDetailCustomization interface
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	//~ End IDetailCustomization interface

private:
	TSharedPtr<IPropertyHandle> MyStructProperty;

	TSharedPtr<SVerticalBox> SimulatedProxiesWidget;
	TSharedPtr<SVerticalBox> AutonomousProxiesWidget;
	SVerticalBox::FSlot* SimulatedProxiesSlot = nullptr;
};
