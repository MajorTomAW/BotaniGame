// Copyright © 2024 Botanibots Team. All rights reserved.


#include "BlueprintFactory_InventoryRowConfig.h"

#include "Rows/GameplayInventoryRowConfig.h"

UBlueprintFactory_InventoryRowConfig::UBlueprintFactory_InventoryRowConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UGameplayInventoryRowConfig::StaticClass();
}

bool UBlueprintFactory_InventoryRowConfig::ConfigureProperties()
{
	return true;
}

FText UBlueprintFactory_InventoryRowConfig::GetDisplayName() const
{
	return NSLOCTEXT("BlueprintFactory_InventoryRowConfig", "CreateInventoryRowConfig", "Inventory Row Config");
}

UObject* UBlueprintFactory_InventoryRowConfig::FactoryCreateNew(
	UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	// check(InParent->IsA(UGameplayInventoryRowConfig::StaticClass()));
	return NewObject<UGameplayInventoryRowConfig>(InParent, InClass, InName, Flags | RF_Transactional, Context);
}
