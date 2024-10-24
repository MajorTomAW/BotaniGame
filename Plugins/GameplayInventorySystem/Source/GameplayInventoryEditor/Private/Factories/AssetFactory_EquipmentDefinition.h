// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "AssetFactory_EquipmentDefinition.generated.h"


class UGameplayEquipmentDefinition;

UCLASS(HideCategories = Object, MinimalAPI)
class UAssetFactory_EquipmentDefinition : public UFactory
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;
	virtual FText GetDisplayName() const override;
	//~ End UFactory Interface

protected:
	UPROPERTY()
	TSubclassOf<UGameplayEquipmentDefinition> SelectedEquipment;
};
