// Copyright © 2024 MajorT. All rights reserved.

#include "AssetFactory_Playset.h"

#include "Playset.h"
#include "Interfaces/IMainFrameModule.h"
#include "Slate/NewPlayset/SNewPlaysetPicker.h"
#include "Slate/NewPlayset/SNewPlaysetPickerWindow.h"

#define LOCTEXT_NAMESPACE "AssetFactory_Playset"

UAssetFactory_Playset::UAssetFactory_Playset(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UPlayset::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

bool UAssetFactory_Playset::ConfigureProperties()
{
	SelectedPlaysetClass = nullptr;
	
	const IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	const TSharedPtr<SWindow> ParentWindow = MainFrame.GetParentWindow();

	SNewPlaysetPicker::FArguments Args;
	Args.AssetClass(UPlayset::StaticClass());

	TSharedRef<SNewPlaysetPickerWindow> NewPlaysetPicker = SNew(SNewPlaysetPickerWindow)
		.NewPlaysetPickerArgs(Args)
		.WindowTitle(LOCTEXT("PlaysetPickerWindowTitle", "Select a Playset Class"));
	FSlateApplication::Get().AddModalWindow(NewPlaysetPicker, ParentWindow);

	if (NewPlaysetPicker.Get().ShouldProceedWithAction())
	{
		UClass* Class = NewPlaysetPicker.Get().GetSelectedClass().Get();
		if (!ensure(Class))
		{
			return false;
		}

		SelectedPlaysetClass = Class;
		return true;
	}

	return false;
}

UObject* UAssetFactory_Playset::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UPlayset* Playset;

	if (SelectedPlaysetClass != nullptr)
	{
		Playset = CreatePlaysetFromClass(SelectedPlaysetClass, InParent, Name, Flags, Context, Warn);
	}
	else
	{
		check(Class->IsChildOf(UPlayset::StaticClass()));
		Playset = CreatePlaysetFromClass(UPlayset::StaticClass(), InParent, Name, Flags, Context, Warn);
	}


	return Playset;
}

FText UAssetFactory_Playset::GetDisplayName() const
{
	return FText::FromString("Playset Item");
}

FString UAssetFactory_Playset::GetDefaultNewAssetName() const
{
	return FString(TEXT("New")) + UPlayset::StaticClass()->GetName();
}

UPlayset* UAssetFactory_Playset::CreatePlaysetFromTemplate(UPlayset* Template, UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UPlayset* Playset = Cast<UPlayset>(StaticDuplicateObject(Template, InParent, InName, Flags, InClass));
	return Playset;
}

UPlayset* UAssetFactory_Playset::CreatePlaysetFromClass(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UPlayset* Playset = NewObject<UPlayset>(InParent, InClass, InName, Flags | RF_Transactional, Context);
	return Playset;
}

void UAssetFactory_Playset::InitializePlayset(UPlayset* Playset)
{
}

TArray<UClass*> UAssetFactory_Playset::GetClasses(UClass* PlaysetClass)
{
	TArray<UClass*> AllClasses;

	AllClasses.AddUnique(PlaysetClass);

	// Find all subclasses of the class
	TArray<UClass*> ChildClasses;
	GetDerivedClasses(PlaysetClass, ChildClasses, true);
	for (UClass* ChildClass : ChildClasses)
	{
		AllClasses.AddUnique(ChildClass);
	}
	
	return AllClasses;
}

#undef LOCTEXT_NAMESPACE