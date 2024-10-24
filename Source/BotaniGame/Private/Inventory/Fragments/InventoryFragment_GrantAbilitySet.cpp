// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/Fragments/InventoryFragment_GrantAbilitySet.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/BotaniAbilitySet.h"
#include "GameFramework/PlayerState.h"
#include "Messaging/BotaniVerbMessageHelpers.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryFragment_GrantAbilitySet)

UInventoryFragment_GrantAbilitySet::UInventoryFragment_GrantAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITOR
FString UInventoryFragment_GrantAbilitySet::GetEditorFriendlyName_Implementation() const
{
	return TEXT("Grant Ability Set");
}
#endif

void UInventoryFragment_GrantAbilitySet::OnItemInstanceCreated(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpec& ItemSpec, class UGameplayInventoryItemInstance* ItemInstance) const
{
	Super::OnItemInstanceCreated(Context, ItemSpec, ItemInstance);

	const APlayerState* PlayerState = UBotaniVerMessageHelpers::GetPlayerStateFromObject(Context.Instigator);
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState);
	if (!ensure(ASC))
	{
		return;
	}

	UBotaniAbilitySystemComponent* BotaniASC = CastChecked<UBotaniAbilitySystemComponent>(ASC);

	for (UBotaniAbilitySet* AbilitySet : AbilitySetsToGrant)
	{
		AbilitySet->GiveToAbilitySystem(BotaniASC, nullptr);
	}
}

void UInventoryFragment_GrantAbilitySet::OnItemInstanceDestroyed(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpec& ItemSpec, class UGameplayInventoryItemInstance* ItemInstance) const
{
	Super::OnItemInstanceDestroyed(Context, ItemSpec, ItemInstance);
}
