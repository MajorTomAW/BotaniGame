// Copyright © 2024 MajorT. All rights reserved.


#include "AssetFactory_EquipmentDefinition.h"

#include "ClassViewerModule.h"
#include "ViewerFilter.h"
#include "Definitions/GameplayEquipmentDefinition.h"
#include "Definitions/GameplayInventoryItemDefinition.h"
#include "Kismet2/SClassPickerDialog.h"
#include "Settings/GameplayInventoryDeveloperSettings.h"

#define LOCTEXT_NAMESPACE "AssetFactory_EquipmentDefinition"

UAssetFactory_EquipmentDefinition::UAssetFactory_EquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UGameplayEquipmentDefinition::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UAssetFactory_EquipmentDefinition::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (SelectedEquipment)
	{
		return NewObject<UGameplayEquipmentDefinition>(InParent, SelectedEquipment, Name, Flags | RF_Transactional, Context);
	}
	check(InParent->IsA(UGameplayEquipmentDefinition::StaticClass()));
	return NewObject<UGameplayEquipmentDefinition>(InParent, Class, Name, Flags | RF_Transactional, Context);
}

bool UAssetFactory_EquipmentDefinition::ConfigureProperties()
{
	// nullptr the class to make sure we don't create a fragment of the wrong type
	SelectedEquipment = nullptr;

	// Load the class viewer module to display the class picker
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.ExtraPickerCommonClasses.Empty();
	Options.DisplayMode = EClassViewerDisplayMode::ListView;
	Options.bAllowViewOptions = true;
	Options.bExpandRootNodes = true;

	TArray<UClass*> Classes;

	for (TSoftClassPtr<class UGameplayEquipmentDefinition> Class : UGameplayInventoryDeveloperSettings::Get()->FavouriteEquipmentDefinitions)
	{
		Classes.Add(Class.LoadSynchronous());
	}
	
	Options.ExtraPickerCommonClasses.Append(Classes);

	TSharedPtr<FInventoryClassViewerFilter> Filter = MakeShared<FInventoryClassViewerFilter>();
	Filter->AllowedChildrenOfClasses.Add(UGameplayEquipmentDefinition::StaticClass());
	Options.ClassFilters.Add(Filter.ToSharedRef());

	const FText TitleText = NSLOCTEXT("AssetFactory_EquipmentDefinition", "CreateEquipmentDefinition", "Pick Equipment Definition Class");
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, UGameplayEquipmentDefinition::StaticClass());

	if (bPressedOk)
	{
		SelectedEquipment = ChosenClass;
	}

	return bPressedOk;
}

FText UAssetFactory_EquipmentDefinition::GetDisplayName() const
{
	return LOCTEXT("AssetFactory_EquipmentDefinition", "Equipment Definition");
}

/*FString UAssetFactory_EquipmentDefinition::GetDefaultNewAssetName() const
{
	return FString(TEXT("New")) + UGameplayEquipmentDefinition::StaticClass()->GetName();
}*/

#undef LOCTEXT_NAMESPACE