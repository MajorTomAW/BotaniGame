// Copyright © 2024 MajorT. All rights reserved.


#include "AssetFactory_InventoryDefinition.h"

#include "ClassViewerModule.h"
#include "ViewerFilter.h"
#include "Definitions/GameplayInventoryItemDefinition.h"
#include "Kismet2/SClassPickerDialog.h"
#include "Settings/GameplayInventoryDeveloperSettings.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UAssetFactory_InventoryDefinition::UAssetFactory_InventoryDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UGameplayInventoryItemDefinition::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
};

bool UAssetFactory_InventoryDefinition::ConfigureProperties()
{
	// nullptr the class to make sure we don't create a fragment of the wrong type
	SelectedItem = nullptr;

	// Load the class viewer module to display the class picker
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.ExtraPickerCommonClasses.Empty();
	Options.DisplayMode = EClassViewerDisplayMode::ListView;
	Options.bAllowViewOptions = true;
	Options.bExpandRootNodes = true;

	TArray<UClass*> Classes;

	for (TSoftClassPtr<class UGameplayInventoryItemDefinition> Class : UGameplayInventoryDeveloperSettings::Get()->FavouriteItemDefinitions)
	{
		Classes.Add(Class.LoadSynchronous());
	}
	
	Options.ExtraPickerCommonClasses.Append(Classes);

	TSharedPtr<FInventoryClassViewerFilter> Filter = MakeShared<FInventoryClassViewerFilter>();
	Filter->AllowedChildrenOfClasses.Add(UGameplayInventoryItemDefinition::StaticClass());
	Options.ClassFilters.Add(Filter.ToSharedRef());

	const FText TitleText = NSLOCTEXT("AssetFactory_EquipmentDefinition", "CreateEquipmentDefinition", "Pick Item Definition Class");
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, UGameplayInventoryItemDefinition::StaticClass());

	if (bPressedOk)
	{
		SelectedItem = ChosenClass;
	}

	return bPressedOk;
}

FText UAssetFactory_InventoryDefinition::GetDisplayName() const
{
	return LOCTEXT("AssetFactory_InventoryDefinition", "Item Definition");
}

FString UAssetFactory_InventoryDefinition::GetDefaultNewAssetName() const
{
	return FString(TEXT("New")) + UGameplayInventoryItemDefinition::StaticClass()->GetName();
}

UObject* UAssetFactory_InventoryDefinition::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	if (SelectedItem)
	{
		return NewObject<UGameplayInventoryItemDefinition>(InParent, SelectedItem, InName, Flags | RF_Transactional, Context);
	}
	check(InParent->IsA(UGameplayInventoryItemDefinition::StaticClass()));
	return NewObject<UGameplayInventoryItemDefinition>(InParent, InClass, InName, Flags | RF_Transactional, Context);
}


#undef LOCTEXT_NAMESPACE
