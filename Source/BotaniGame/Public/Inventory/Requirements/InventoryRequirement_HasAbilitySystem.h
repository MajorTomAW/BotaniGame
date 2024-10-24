// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Requirements/GameplayInventoryRequirement.h"
#include "InventoryRequirement_HasAbilitySystem.generated.h"


UCLASS(meta = (DisplayName = "Has Ability System"), MinimalAPI)
class UInventoryRequirement_HasAbilitySystem : public UGameplayInventoryRequirement
{
	GENERATED_BODY()

public:
	UInventoryRequirement_HasAbilitySystem();

	//~ Begin UGameplayInventoryRequirement Interface
	BOTANIGAME_API virtual bool CanInventoryItemPerformAction(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& Context) const override;
	//~ End UGameplayInventoryRequirement Interface

/*#if WITH_EDITOR
	BOTANIGAME_API virtual FString GetEditorFriendlyName_Implementation() const override;
#endif*/
};
