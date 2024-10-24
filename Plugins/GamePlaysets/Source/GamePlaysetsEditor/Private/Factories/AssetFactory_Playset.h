// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "AssetFactory_Playset.generated.h"

class UPlayset;

UCLASS(HideCategories = Object, MinimalAPI)
class UAssetFactory_Playset : public UFactory
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UFactory Interface
	GAMEPLAYSETSEDITOR_API virtual bool ConfigureProperties() override;
	GAMEPLAYSETSEDITOR_API virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	GAMEPLAYSETSEDITOR_API virtual FText GetDisplayName() const override;
	GAMEPLAYSETSEDITOR_API virtual FString GetDefaultNewAssetName() const override;
	//~ End UFactory Interface
	
private:
	UPlayset* CreatePlaysetFromTemplate(UPlayset* Template, UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn);
	UPlayset* CreatePlaysetFromClass(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn);

	void InitializePlayset(UPlayset* Playset);
	static TArray<UClass*> GetClasses(UClass* PlaysetClass);

private:
	UPROPERTY()
	TSubclassOf<UPlayset> SelectedPlaysetClass;
};