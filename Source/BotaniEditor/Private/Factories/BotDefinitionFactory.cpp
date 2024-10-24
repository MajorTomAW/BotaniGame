// Copyright © 2024 Botanibots Team. All rights reserved.


#include "BotDefinitionFactory.h"

#include "Bot/BotaniBotDefinition.h"

UBotDefinitionFactory::UBotDefinitionFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UBotaniBotDefinition::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

bool UBotDefinitionFactory::ConfigureProperties()
{
	return true;
}

UObject* UBotDefinitionFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	check(InClass->IsChildOf(UBotaniBotDefinition::StaticClass()));
	return NewObject<UBotaniBotDefinition>(InParent, InClass, InName, Flags);
}
