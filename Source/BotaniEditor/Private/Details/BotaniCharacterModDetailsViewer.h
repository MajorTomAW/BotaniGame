// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

class FBotaniCharacterModDetailsViewer : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	//~ Begin IDetailCustomization interface
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	//~ End IDetailCustomization interface

private:
	TSharedPtr<IPropertyHandle> MyPropertyHandle;

private:
	TSharedRef<SWidget> OnGetModContent() const;
	FText GetCurrentModDesc() const;
	void OnModComboChange(int32 Index);

	TArray<FName> ModValues;
};