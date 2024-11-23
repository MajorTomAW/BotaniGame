// Copyright © 2024 MajorT. All rights reserved.


#include "BlueprintFactory_InventoryFragment.h"

#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "KismetCompilerModule.h"
#include "ViewerFilter.h"
#include "Fragments/GameplayEquipmentFragment.h"
#include "Fragments/GameplayInventoryItemFragment.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/SClassPickerDialog.h"

#include "Fragments/GameplayEquipmentFragment.h"
#include "Fragments/GameplayInventoryItemFragment.h"
#include "Fragments/GameplayInventorySystemFragment.h"

#define LOCTEXT_NAMESPACE "BlueprintFactory_InventoryFragment"


//////////////////////////////////////////////////////////////////////////
/// UBlueprintFactory_InventoryFragment

UBlueprintFactory_InventoryFragment::UBlueprintFactory_InventoryFragment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, FragmentClass(nullptr)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = nullptr;
	ParentClass = nullptr;
};

bool UBlueprintFactory_InventoryFragment::ConfigureProperties()
{
	// nullptr the class to make sure we don't create a fragment of the wrong type
	SelectedFragmentClass = nullptr;

	// Load the class viewer module to display the class picker
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.ExtraPickerCommonClasses.Empty();
	Options.DisplayMode = EClassViewerDisplayMode::ListView;
	Options.bAllowViewOptions = true;
	Options.bExpandRootNodes = true;
	Options.ExtraPickerCommonClasses.Add(FragmentClass);

	TSharedPtr<FInventoryClassViewerFilter> Filter = MakeShared<FInventoryClassViewerFilter>();
	Filter->AllowedChildrenOfClasses.Add(FragmentClass);
	Options.ClassFilters.Add(Filter.ToSharedRef());

	const FText TitleText = NSLOCTEXT("CreateInventoryFragment", "CreateInventoryFragment", "Pick Fragment Class");
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, FragmentClass);

	if (bPressedOk)
	{
		SelectedFragmentClass = ChosenClass;
	}

	return bPressedOk;
}

FText UBlueprintFactory_InventoryFragment::GetDisplayName() const
{
	return FragmentClass ? FragmentClass->GetDisplayNameText() : LOCTEXT("FragmentBlueprint", "Fragment Blueprint");
}

UObject* UBlueprintFactory_InventoryFragment::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	UClass* ClassToUse = SelectedFragmentClass ? SelectedFragmentClass.Get() : FragmentClass;

	if (ClassToUse == nullptr && !FKismetEditorUtilities::CanCreateBlueprintOfClass(ClassToUse))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ClassName"), (ClassToUse != nullptr) ? FText::FromString(ClassToUse->GetName()) : LOCTEXT("Null", "null"));
        
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(LOCTEXT("CannotCreateBlueprint", "Cannot create a blueprint based on class '{ClassName}'."), Args));
		return nullptr;
	}

	UClass* BlueprintClass = nullptr;
	UClass* BlueprintGeneratedClass = nullptr;

	IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
	KismetCompilerModule.GetBlueprintTypesForClass(ClassToUse, BlueprintClass, BlueprintGeneratedClass);
	return FKismetEditorUtilities::CreateBlueprint(ClassToUse, InParent, InName, BlueprintType, BlueprintClass, BlueprintGeneratedClass, CallingContext);
}


//////////////////////////////////////////////////////////////////////////
/// UBlueprintFactory_EquipmentFragment

UBlueprintFactory_EquipmentFragment::UBlueprintFactory_EquipmentFragment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FragmentClass = UGameplayEquipmentFragment::StaticClass();
	SupportedClass = UGameplayEquipmentFragment::StaticClass();
	ParentClass = UGameplayEquipmentFragment::StaticClass();
};

//////////////////////////////////////////////////////////////////////////
/// UBlueprintFactory_ItemFragment

UBlueprintFactory_ItemFragment::UBlueprintFactory_ItemFragment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FragmentClass = UGameplayInventoryItemFragment::StaticClass();
	SupportedClass = UGameplayInventoryItemFragment::StaticClass();
	ParentClass = UGameplayInventoryItemFragment::StaticClass();
};

#undef LOCTEXT_NAMESPACE