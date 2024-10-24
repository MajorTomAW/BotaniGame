// Copyright © 2024 Botanibots Team. All rights reserved.


#include "BotaniCharacterModDetailsViewer.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"

TSharedRef<IPropertyTypeCustomization> FBotaniCharacterModDetailsViewer::MakeInstance()
{
	return MakeShared<FBotaniCharacterModDetailsViewer>();
}

void FBotaniCharacterModDetailsViewer::CustomizeHeader(
	TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	MyPropertyHandle = PropertyHandle;

	UE_LOG(LogTemp, Error, TEXT("Customizing Property: %s"), *PropertyHandle.Get().GetPropertyDisplayName().ToString())
	
	HeaderRow.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget(FText::FromString("Character Mod"), FText::FromString("Character Mod Tooltip"))
	]
	.ValueContent()
	[
		SNew(SComboButton)
		.OnGetMenuContent(this, &FBotaniCharacterModDetailsViewer::OnGetModContent)
		.ContentPadding(FMargin(2.f))
		.ButtonContent()
		[
			SNew(STextBlock)
			.Font(IDetailLayoutBuilder::GetDetailFont())
			.Text(this, &FBotaniCharacterModDetailsViewer::GetCurrentModDesc)
		]
	];
}

TSharedRef<SWidget> FBotaniCharacterModDetailsViewer::OnGetModContent() const
{
	FMenuBuilder MenuBuilder(true, nullptr);

	for (int32 Idx = 0; Idx < ModValues.Num(); Idx++)
	{
		FUIAction ItemAction(FExecuteAction::CreateSP(const_cast<FBotaniCharacterModDetailsViewer*>(this),
			&FBotaniCharacterModDetailsViewer::OnModComboChange, Idx));

		MenuBuilder.AddMenuEntry(FText::FromName(ModValues[Idx]), TAttribute<FText>(), FSlateIcon(), ItemAction);
	}

	return MenuBuilder.MakeWidget();
}

FText FBotaniCharacterModDetailsViewer::GetCurrentModDesc() const
{
	FName NameValue;
	if (MyPropertyHandle)
	{
		MyPropertyHandle->GetValue(NameValue);
	}

	const int32 ModIx = ModValues.IndexOfByKey(NameValue);
	if (ModIx != INDEX_NONE && ModValues.IsValidIndex(ModIx))
	{
		return FText::FromName(ModValues[ModIx]);
	}

	return FText::FromName(NameValue);
}

void FBotaniCharacterModDetailsViewer::OnModComboChange(int32 Index)
{
}

void FBotaniCharacterModDetailsViewer::CustomizeChildren(
	TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// ChildBuilder.GetParentGroup()->AddPropertyRow(PropertyHandle);
}
