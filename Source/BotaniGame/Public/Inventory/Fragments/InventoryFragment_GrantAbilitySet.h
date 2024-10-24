// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Fragments/GameplayInventoryItemFragment.h"
#include "InventoryFragment_GrantAbilitySet.generated.h"

class UBotaniAbilitySet;

UCLASS(meta = (DisplayName = "Grant Ability Set"))
class BOTANIGAME_API UInventoryFragment_GrantAbilitySet : public UGameplayInventoryItemFragment
{
	GENERATED_UCLASS_BODY()

public:
	/** Ability sets to grant when this is equipped */
	UPROPERTY(EditDefaultsOnly, Category = "Ability Set")
	TArray<TObjectPtr<UBotaniAbilitySet>> AbilitySetsToGrant;

public:
	//~ Begin UGameplayInventoryItemFragment Interface
	virtual void OnItemInstanceCreated(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpec& ItemSpec, class UGameplayInventoryItemInstance* ItemInstance) const override;
	virtual void OnItemInstanceDestroyed(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpec& ItemSpec, class UGameplayInventoryItemInstance* ItemInstance) const override;
	//~ End UGameplayInventoryItemFragment Interface

protected:
	//~ Begin UGameplayInventoryItemFragment Interface
#if WITH_EDITOR
	virtual FString GetEditorFriendlyName_Implementation() const override;
#endif
	//~ End UGameplayInventoryItemFragment Interface
};
