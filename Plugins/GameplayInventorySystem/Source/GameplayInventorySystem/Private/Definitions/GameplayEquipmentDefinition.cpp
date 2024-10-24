// Copyright © 2024 MajorT. All rights reserved.


#include "Definitions/GameplayEquipmentDefinition.h"

#if WITH_EDITOR
#include "UObject/ObjectSaveContext.h"
#include "Misc/DataValidation.h"
#endif

#include "Instance/GameplayEquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayEquipmentDefinition)

#define LOCTEXT_NAMESPACE "GameplayInventorySystem"


UGameplayEquipmentDefinition::UGameplayEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, DefaultInstanceClass(UGameplayEquipmentInstance::StaticClass())
{
}

UClass* UGameplayEquipmentDefinition::GetInstanceClass() const
{
	return EquipmentInstanceClass ? EquipmentInstanceClass : DefaultInstanceClass;
}

#if WITH_EDITOR
EDataValidationResult UGameplayEquipmentDefinition::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = UObject::IsDataValid(Context);

	if (ActorSpawnRule == nullptr)
	{
		Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
		Context.AddError(LOCTEXT("ActorToSpawnRuleInvalid", "Actor to spawn rule is invalid."));
	}

	return Result;
}

void UGameplayEquipmentDefinition::PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext)
{
	UObject::PostSaveRoot(ObjectSaveContext);
}
#endif

#undef LOCTEXT_NAMESPACE