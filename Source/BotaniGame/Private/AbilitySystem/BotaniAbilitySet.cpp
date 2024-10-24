// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/BotaniAbilitySet.h"

#include "BotaniLogChannels.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#define LOCTEXT_NAMESPACE "BotaniAbilitySet"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniAbilitySet)


//////////////////////////////////////////////////////////////////////////////
/// FBotaniAbilitySet_GrantedHandles

void FBotaniAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& AbilityHandle)
{
	if (!AbilityHandle.IsValid())
	{
		return;
	}

	GrantedAbilityHandles.Add(AbilityHandle);
}

void FBotaniAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& EffectHandle)
{
	if (!EffectHandle.IsValid())
	{
		return;
	}

	GrantedEffectHandles.Add(EffectHandle);
}

void FBotaniAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* AttributeSet)
{
	if (!AttributeSet)
	{
		return;
	}
	
	GrantedAttributeSets.Add(AttributeSet);
}

void FBotaniAbilitySet_GrantedHandles::TakeFromAbilitySystem(UBotaniAbilitySystemComponent* BotaniASC)
{
	check(BotaniASC);

	if (!BotaniASC->IsOwnerActorAuthoritative())
	{
		// Only authoritative actors can give or take ability sets.
		return;
	}

	// Remove granted abilities.
	for (const FGameplayAbilitySpecHandle& Handle : GrantedAbilityHandles)
	{
		if (Handle.IsValid())
		{
			BotaniASC->ClearAbility(Handle);
		}
	}

	// Remove granted effects.
	for (const FActiveGameplayEffectHandle& Handle : GrantedEffectHandles)
	{
		if (Handle.IsValid())
		{
			BotaniASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	// Remove granted attribute sets.
	for (UAttributeSet* AttributeSet : GrantedAttributeSets)
	{
		BotaniASC->RemoveSpawnedAttribute(AttributeSet);
	}

	GrantedAbilityHandles.Reset();
	GrantedEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}


//////////////////////////////////////////////////////////////////////////
/// UBotaniAbilitySet

UBotaniAbilitySet::UBotaniAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBotaniAbilitySet::GiveToAbilitySystem(UBotaniAbilitySystemComponent* BotaniASC, FBotaniAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(BotaniASC);

	if (!BotaniASC->IsOwnerActorAuthoritative())
	{
		// Only authoritative actors can give or take ability sets.
		return;
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedAbilities.Num(); ++AbilityIndex)
	{
		const FBotaniAbilitySet_GameplayAbility& AbilityToGrant = GrantedAbilities[AbilityIndex];

		if (AbilityToGrant.Ability.IsNull())
		{
			BOTANI_GAS_LOG(Error, TEXT("UBotaniAbilitySet::GiveToAbilitySystem: Invalid gameplay ability in granted abilities list."));
			continue;
		}

		const UClass* AbilityClass = AbilityToGrant.Ability.LoadSynchronous();
		UBotaniGameplayAbility* AbilityCDO = AbilityClass->GetDefaultObject<UBotaniGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle Handle = BotaniASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(Handle);
		}
	}

	// Grant the gameplay effects
	for (int32 EffectIndex = 0; EffectIndex < GrantedEffects.Num(); ++EffectIndex)
	{
		const FBotaniAbilitySet_GameplayEffect& EffectToGrant = GrantedEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			BOTANI_GAS_LOG(Error, TEXT("UBotaniAbilitySet::GiveToAbilitySystem: Invalid gameplay effect in granted effects list."));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle Handle = BotaniASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, BotaniASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(Handle);
		}
	}

	// Grant the attribute sets
	for (int32 AttributeSetIndex = 0; AttributeSetIndex < GrantedAttributeSets.Num(); ++AttributeSetIndex)
	{
		const FBotaniAbilitySet_AttributeSet& AttributeSetToGrant = GrantedAttributeSets[AttributeSetIndex];

		if (!IsValid(AttributeSetToGrant.AttributeSet))
		{
			BOTANI_GAS_LOG(Error, TEXT("UBotaniAbilitySet::GiveToAbilitySystem: Invalid attribute set in granted attribute sets list."));
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(BotaniASC->GetOwner(), AttributeSetToGrant.AttributeSet);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}

#if WITH_EDITOR
EDataValidationResult UBotaniAbilitySet::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	// Validate granted abilities
	for (const FBotaniAbilitySet_GameplayAbility& Ability : GrantedAbilities)
	{
		if (!Ability.Ability)
		{
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
			Context.AddError(FText::Format(LOCTEXT("InvalidGrantedAbility", "Invalid granted ability class in {0}."), FText::FromString(GetPathName())));
		}

		/*if (!Ability.InputTag.IsValid())
		{
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
			Context.AddError(FText::Format(LOCTEXT("InvalidGrantedAbilityInputTag", "Invalid granted ability input tag in {0}."), FText::FromString(GetPathName())));
		}*/
	}

	// Validate granted effects
	for (const FBotaniAbilitySet_GameplayEffect& Effect : GrantedEffects)
	{
		if (!Effect.GameplayEffect)
		{
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
			Context.AddError(FText::Format(LOCTEXT("InvalidGrantedEffect", "Invalid granted effect class in {0}."), FText::FromString(GetPathName())));
		}
	}

	// Validate granted attribute sets
	for (const FBotaniAbilitySet_AttributeSet& AttributeSet : GrantedAttributeSets)
	{
		if (!AttributeSet.AttributeSet)
		{
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
			Context.AddError(FText::Format(LOCTEXT("InvalidGrantedAttributeSet", "Invalid granted attribute set class in {0}."), FText::FromString(GetPathName())));
		}
	}
	
	return Result;
}
#endif
#undef LOCTEXT_NAMESPACE