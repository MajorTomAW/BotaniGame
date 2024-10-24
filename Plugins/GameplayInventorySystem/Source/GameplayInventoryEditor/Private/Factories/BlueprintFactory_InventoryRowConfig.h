// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintFactory_InventoryRowConfig.generated.h"

UCLASS()
class UBlueprintFactory_InventoryRowConfig : public UFactory
{
	GENERATED_UCLASS_BODY()

public:
	virtual bool ConfigureProperties() override;
	virtual FText GetDisplayName() const override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
};
