// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "GameplayAbilitySpecHandle.h"
#include "UObject/Object.h"
#include "BotaniAbilityCost.generated.h"

struct FGameplayTagContainer;
struct FGameplayAbilityActorInfo;
class UBotaniGameplayAbility;
/**
 * UBotaniAbilityCost
 *
 * Base class for costs that a BotaniGameplayAbility has (e.g., ammo or charges)
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract, Blueprintable, BlueprintType)
class BOTANIGAME_API UBotaniAbilityCost : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/**
	 * Checks if we can afford this ability cost.
	 *
	 * A failure reason tag can be added to OptionalRelevantTags (if non-null), which can be queried elsewhere to determine how to provide user feedback (e.g., a clicking noise if a weapon is out of ammo)
	 * Ability and ActorInfo are guaranteed to be non-null on entry, but OptionalRelevantTags can be nullptr.
	 * 
	 * @param Ability	The ability that is being checked for cost. 
	 * @param Handle	The handle to the ability that is being checked for cost.
	 * @param ActorInfo		The current actor info of the ability, note that the ability needs to be set InstancedPerActor.
	 * @param OptionalRelevantTags	Optional tags that can be used to provide user feedback.
	 * @return	true if we can pay for the ability, false otherwise.
	 */
	virtual bool CheckCost(const UBotaniGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const;

	/**
	 * Checks if we can afford this ability cost.
	 *
	 * A failure reason tag can be added to OptionalRelevantTags (if non-null), which can be queried elsewhere to determine how to provide user feedback (e.g., a clicking noise if a weapon is out of ammo)
	 * Ability and ActorInfo are guaranteed to be non-null on entry, but OptionalRelevantTags can be nullptr.
	 * 
	 * @param Ability	The ability that is being checked for cost. 
	 * @param Handle	The handle to the ability that is being checked for cost.
	 * @param ActorInfo		The current actor info of the ability, note that the ability needs to be set InstancedPerActor.
	 * @param OptionalRelevantTags	Optional tags that can be used to provide user feedback.
	 * @return	true if we can pay for the ability, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Cost", meta = (DisplayName = "Check Cost"))
	bool K2_CheckCost(const UBotaniGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, FGameplayTagContainer& OptionalRelevantTags) const;

	/**
	 * Applies the ability's cost to the target
	 *
	 * Notes:
-
	 *
	 *  Your implementation doesn't need to check ShouldOnlyApplyCostOnHit(), the caller does that for you.
	 *
	 * – Ability and ActorInfo are guaranteed to be non-null on entry.
	 */
	virtual void ApplyCost(const UBotaniGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);

	/**
	 * Applies the ability's cost to the target
	 *
	 * Notes:
-
	 *
	 *  Your implementation doesn't need to check ShouldOnlyApplyCostOnHit(), the caller does that for you.
	 *
	 * – Ability and ActorInfo are guaranteed to be non-null on entry.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Cost", meta = (DisplayName = "Apply Cost"))
	void K2_ApplyCost(const UBotaniGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo);

	/**
	 * If true, this cost should only be applied if this ability hits successfully
	 */
	virtual bool ShouldOnlyApplyCostOnHit() const { return bOnlyApplyCostOnHit; }

protected:
	/** If true, this cost should only be applied if this ability hits successfully */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Costs")
	uint8 bOnlyApplyCostOnHit:1;

/*
protected:
#if WITH_EDITORONLY_DATA
	/** The Editor display name of the cost #1#
	UPROPERTY(EditAnywhere, Category = "Costs", meta = (DisplayName = "Display Name", EditCondition = "false", EditConditionHides))
	FText EditorDisplayName;

	/** Gets the display name of the cost #1#
	UFUNCTION(BlueprintNativeEvent, Category = "Cost", meta = (DisplayName = "Get Display Name"))
	FText K2_GetDisplayName() const;
#endif*/
};
