// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GamePlaysetsEditor/Private/Factories/AssetFactory_Playset.h"
#include "AssetFactory_InventoryDefinition.generated.h"


class UGameplayInventoryItemDefinition;

UCLASS()
class GAMEPLAYINVENTORYEDITOR_API UAssetFactory_InventoryDefinition : public UAssetFactory_Playset
{
	GENERATED_UCLASS_BODY()

	//~ Begin UFactory Interface
	virtual bool ConfigureProperties() override;
	virtual FText GetDisplayName() const override;
	virtual FString GetDefaultNewAssetName() const override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	//~ End UFactory Interface

protected:
	UPROPERTY()
	TSubclassOf<UGameplayInventoryItemDefinition> SelectedItem;
};
