// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniGameplayAbility.h"
#include "BotaniGameplayAbility_FromEquipment.generated.h"

/**
 * UBotaniGameplayAbility_FromEquipment
 *
 * An ability granted by an associated equipment instance
 */
UCLASS(meta = (ShortTooltip = "An ability granted by an associated equipment instance.", DisplayName = "Equipment Ability"))
class BOTANIGAME_API UBotaniGameplayAbility_FromEquipment : public UBotaniGameplayAbility
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UGameplayAbility Interface
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	//~ End UGameplayAbility Interface

public:
	/** Returns the associated item instance */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Equipment")
	class UBotaniItemInstance* GetAssociatedItem() const;

	/** Returns the associated equipment instance */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Equipment")
	class UBotaniEquipmentInstance* GetAssociatedEquipment() const;

	/** Returns the owning inventory manager */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Equipment")
	class UGameplayInventoryManager* GetInventoryManager() const;

	/** Returns the owning equipment manager */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Equipment")
	class UGameplayEquipmentManager* GetEquipmentManager() const;
	

protected:
#if WITH_EDITOR
	//~ Begin UObject Interface
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	//~ End UObject Interface
#endif
};
