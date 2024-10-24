// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniAbilitySystemComponent.h"
#include "Engine/DataAsset.h"
#include "BotaniAbilitySet.generated.h"

/**
 * FBotaniAbilitySet_GameplayAbility
 *
 * Data used by the ability set to grant gameplay abilities.
 */
USTRUCT(BlueprintType)
struct FBotaniAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:
	/** Gameplay ability to grant. */
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySet")
	TSoftClassPtr<UBotaniGameplayAbility> Ability = nullptr;

	/** Level of ability to grant. */
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySet")
	int32 AbilityLevel = 1;

	/** Tag used to process input for the ability. */
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySet", meta = (Categories = "Input.InputTag"))
	FGameplayTag InputTag;
};

/**
 * FBotaniAbilitySet_GameplayEffect
 *
 * Data used by the ability set to grant gameplay effects.
 */
USTRUCT(BlueprintType)
struct FBotaniAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:
	/** Gameplay effect to grant. */
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySet")
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	/** Level of effect to grant. */
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySet")
	float EffectLevel = 1.f;
};

/**
 * FBotaniAbilitySet_AttributeSet
 *
 * Data used by the ability set to grant attribute sets.
 */
USTRUCT(BlueprintType)
struct FBotaniAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	/** Attribute set to grant. */
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySet")
	TSubclassOf<UAttributeSet> AttributeSet = nullptr;
};

/**
 * FBotaniAbilitySet_GrantedHandles
 *
 * Data used to store granted abilities, effects and attribute sets.
 */
USTRUCT(BlueprintType)
struct FBotaniAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	FBotaniAbilitySet_GrantedHandles() = default;
	explicit FBotaniAbilitySet_GrantedHandles(TArray<FGameplayAbilitySpecHandle>& AbilityHandles, TArray<FActiveGameplayEffectHandle>& EffectHandles, TArray<TObjectPtr<UAttributeSet>>& AttributeSets)
		: GrantedAbilityHandles(MoveTemp(AbilityHandles))
		, GrantedEffectHandles(MoveTemp(EffectHandles))
		, GrantedAttributeSets(MoveTemp(AttributeSets))
	{
	}

public:
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& AbilityHandle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& EffectHandle);
	void AddAttributeSet(UAttributeSet* AttributeSet);

	BOTANIGAME_API void TakeFromAbilitySystem(UBotaniAbilitySystemComponent* BotaniASC);
	BOTANIGAME_API const TArray<FGameplayAbilitySpecHandle>& GetAbilitySpecHandles() const { return GrantedAbilityHandles; }
	BOTANIGAME_API const TArray<FActiveGameplayEffectHandle>& GetGameplayEffectHandles() const { return GrantedEffectHandles; }
	BOTANIGAME_API const TArray<UAttributeSet*>& GetAttributeSets() const { return GrantedAttributeSets; }

protected:
	/** Handles to granted abilities. */
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;

	/** Handles to granted effects. */
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GrantedEffectHandles;

	/** Granted attribute sets. */
	UPROPERTY()
	TArray<UAttributeSet*> GrantedAttributeSets;
};


/**
 * UBotaniAbilitySet
 *
 * Non-mutable data asset used to grant gameplay abilities, effects and attribute sets.
 */
UCLASS(BlueprintType, Const, MinimalAPI)
class UBotaniAbilitySet final : public UPrimaryDataAsset
{
	GENERATED_UCLASS_BODY()

public:
	/** Grants the ability set to the specific asc. The returned handles can be used later to take away anything that was granted. */
	BOTANIGAME_API void GiveToAbilitySystem(UBotaniAbilitySystemComponent* BotaniASC, FBotaniAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;

protected:
	/** Abilities to grant when this ability set is granted. */
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySet", meta = (TitleProperty = Ability))
	TArray<FBotaniAbilitySet_GameplayAbility> GrantedAbilities;

	/** Effects to grant when this ability set is granted. */
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySet", meta = (TitleProperty = GameplayEffect))
	TArray<FBotaniAbilitySet_GameplayEffect> GrantedEffects;

	/** Attribute sets to grant when this ability set is granted. */
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySet", meta = (TitleProperty = AttributeSet))
	TArray<FBotaniAbilitySet_AttributeSet> GrantedAttributeSets;

protected:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
