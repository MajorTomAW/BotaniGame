// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/BlueprintFactory.h"
#include "BlueprintFactory_InventoryFragment.generated.h"


UCLASS()
class GAMEPLAYINVENTORYEDITOR_API UBlueprintFactory_InventoryFragment : public UBlueprintFactory
{
	GENERATED_UCLASS_BODY()

public:
	virtual bool ConfigureProperties() override;
	virtual FText GetDisplayName() const override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;

protected:
	UClass* FragmentClass;

	UPROPERTY()
	TObjectPtr<UClass> SelectedFragmentClass = nullptr;
};


UCLASS()
class UBlueprintFactory_EquipmentFragment : public UBlueprintFactory_InventoryFragment
{
	GENERATED_UCLASS_BODY()
};

UCLASS()
class UBlueprintFactory_ItemFragment : public UBlueprintFactory_InventoryFragment
{
	GENERATED_UCLASS_BODY()
};
