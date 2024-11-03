// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/Abilities/BotaniGameplayAbility_FromEquipment.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include "BotaniLogChannels.h"
#include "Inventory/Instances/BotaniEquipmentInstance.h"
#include "Inventory/Instances/BotaniItemInstance.h"
#include "Library/InventorySystemBlueprintLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniGameplayAbility_FromEquipment)

UBotaniGameplayAbility_FromEquipment::UBotaniGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UBotaniGameplayAbility_FromEquipment::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const UBotaniItemInstance* AssociatedItem = GetAssociatedItem();
	if (AssociatedItem == nullptr)
	{
		BOTANI_GAS_LOG(Error, TEXT("Equipment ability %s cannot be activated because there is no associated item (%s)"),
			*GetName(),
			*GetNameSafe(GetAssociatedEquipment()))
			
		return false;
	}

	return true;
}


#if WITH_EDITOR
EDataValidationResult UBotaniGameplayAbility_FromEquipment::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
	{
		Context.AddError(NSLOCTEXT("Botani", "EquipmentAbilityMustBeInstanced", "Equipment ability must be instanced"));
		Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
	}

	return Result;
}
#endif


UBotaniItemInstance* UBotaniGameplayAbility_FromEquipment::GetAssociatedItem() const
{
	UBotaniEquipmentInstance* Equipment = GetAssociatedEquipment();
	if (Equipment != nullptr)
	{
		UObject* Instigator = Equipment->GetInstigator();
		return Cast<UBotaniItemInstance>(Instigator);
	}

	return nullptr;
}



class UBotaniEquipmentInstance* UBotaniGameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	if (Spec != nullptr)
	{
		UObject* SourceObj = Spec->SourceObject.Get();
		return Cast<UBotaniEquipmentInstance>(SourceObj);
	}

	return nullptr;
}

class UGameplayInventoryManager* UBotaniGameplayAbility_FromEquipment::GetInventoryManager() const
{
	UGameplayInventoryManager* InventoryManager = UInventorySystemBlueprintLibrary::FindInventoryManager(GetControllerFromActorInfo());

	if (InventoryManager == nullptr)
	{
		InventoryManager = UInventorySystemBlueprintLibrary::FindInventoryManager(GetAvatarActorFromActorInfo());
	}
	
	return InventoryManager;
}

class UGameplayEquipmentManager* UBotaniGameplayAbility_FromEquipment::GetEquipmentManager() const
{
	UGameplayEquipmentManager* EquipmentManager = UInventorySystemBlueprintLibrary::FindEquipmentManager(GetControllerFromActorInfo());

	if (EquipmentManager == nullptr)
	{
		EquipmentManager = UInventorySystemBlueprintLibrary::FindEquipmentManager(GetAvatarActorFromActorInfo());
	}

	return EquipmentManager;
}