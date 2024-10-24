// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "BotDefinitionFactory.generated.h"

/**
 * 
 */
UCLASS()
class UBotDefinitionFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
};
