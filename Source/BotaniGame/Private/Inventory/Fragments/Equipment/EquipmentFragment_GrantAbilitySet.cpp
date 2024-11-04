// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/Fragments/Equipment/EquipmentFragment_GrantAbilitySet.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/BotaniAbilitySet.h"
#include "AbilitySystem/BotaniAbilitySystemComponent.h"
#include "Instance/GameplayEquipmentInstance.h"

UEquipmentFragment_GrantAbilitySet::UEquipmentFragment_GrantAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UEquipmentFragment_GrantAbilitySet::CanApplyInventoryFragment(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, FGameplayTagContainer* OutBlockingTags) const
{
	if (!Context.Instigator || !Context.HasInstigatorOrOwnerAuthority())
	{
		return false;
	}
	
	return Super::CanApplyInventoryFragment(Context, Handle, OutBlockingTags);
}

void UEquipmentFragment_GrantAbilitySet::OnEquipmentInstanceCreated(const FGameplayInventoryItemContext& Context, const FGameplayEquipmentSpec& EquipmentSpec, class UGameplayEquipmentInstance* EquipmentInstance) const
{
	Super::OnEquipmentInstanceCreated(Context, EquipmentSpec, EquipmentInstance);

	const AActor* OwnerActor = EquipmentInstance->GetPawn();
	check(OwnerActor);

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor);
	UBotaniAbilitySystemComponent* BotaniASC = CastChecked<UBotaniAbilitySystemComponent>(ASC);

	FBotaniAbilitySet_GrantedHandles GrantedHandles;

	// Apply the ability sets to the owner
	for (const UBotaniAbilitySet* Set : AbilitySetsToGrant)
	{
		if (Set == nullptr)
		{
			continue;
		}

		
		Set->GiveToAbilitySystem(BotaniASC, &GrantedHandles, EquipmentInstance);
	}

	// Apply the abilities to the owner
	for (const TSoftClassPtr<UGameplayAbility>& AbilityClass : AbilitiesToGrant)
	{
		if (AbilityClass.IsNull())
		{
			continue;
		}

		FGameplayAbilitySpec NewSpec(AbilityClass.LoadSynchronous());
		FGameplayAbilitySpecHandle Handle = BotaniASC->GiveAbility(NewSpec);
		GrantedHandles.AddAbilitySpecHandle(Handle);
	}

	// Apply the effects to the owner
	for (const TSoftClassPtr<UGameplayEffect>& EffectClass : EffectsToApply)
	{
		if (EffectClass.IsNull())
		{
			continue;
		}

		FGameplayEffectSpecHandle SpecHandle = BotaniASC->MakeOutgoingSpec(EffectClass.LoadSynchronous(), 1.0f, BotaniASC->MakeEffectContext());
		FActiveGameplayEffectHandle Handle =  BotaniASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		GrantedHandles.AddGameplayEffectHandle(Handle);
	}

	FGameplayEquipmentGrantedHandles EquipmentGrantedHandles;
	EquipmentGrantedHandles.AbilityHandles = GrantedHandles.GetAbilitySpecHandles();
	EquipmentGrantedHandles.EffectHandles = GrantedHandles.GetGameplayEffectHandles();
	EquipmentGrantedHandles.AttributeSets.Append(GrantedHandles.GetAttributeSets());

	FGameplayEquipmentSpec& MutableSpec = const_cast<FGameplayEquipmentSpec&>(EquipmentSpec);
	MutableSpec.SetGrantedHandles(EquipmentGrantedHandles);
}

void UEquipmentFragment_GrantAbilitySet::OnEquipmentInstanceDestroyed(const FGameplayInventoryItemContext& Context, const FGameplayEquipmentSpec& EquipmentSpec, class UGameplayEquipmentInstance* EquipmentInstance) const
{
	Super::OnEquipmentInstanceDestroyed(Context, EquipmentSpec, EquipmentInstance);

	const AActor* OwnerActor = EquipmentInstance->GetPawn();
	check(OwnerActor);

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor);
	UBotaniAbilitySystemComponent* BotaniASC = CastChecked<UBotaniAbilitySystemComponent>(ASC);

	// Remove the ability sets from the owner
	FBotaniAbilitySet_GrantedHandles GrantedHandles;

	for (auto& Handle : EquipmentSpec.GrantedHandles.AbilityHandles)
	{
		GrantedHandles.AddAbilitySpecHandle(Handle);
	}

	for (auto& Handle : EquipmentSpec.GrantedHandles.EffectHandles)
	{
		GrantedHandles.AddGameplayEffectHandle(Handle);
	}

	for (auto& Set : EquipmentSpec.GrantedHandles.AttributeSets)
	{
		GrantedHandles.AddAttributeSet(const_cast<UAttributeSet*>(Set.Get()));
	}

	GrantedHandles.TakeFromAbilitySystem(BotaniASC);
}
