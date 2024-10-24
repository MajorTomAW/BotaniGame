// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/Attributes/BotaniHealthSet.h"

#include "GameplayEffectExtension.h"
#include "AbilitySystem/BotaniAbilitySystemComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayTags/BotaniGameplayTags.h"
#include "Messaging/BotaniVerbMessage.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniHealthSet)

UBotaniHealthSet::UBotaniHealthSet()
	: Health(0.f)
	, MaxHealth(0.f)
	, Shield(0.f)
	, MaxShield(0.f)
	, Healing(0.f)
	, Damage(0.f)
{
	bOutOfHealth = false;
	MaxHealthBeforeAttributeChange = 0.f;
	HealthBeforeAttributeChange = 0.f;
}

void UBotaniHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxShield, COND_None, REPNOTIFY_Always);
}

void UBotaniHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldValue);

	// Call the change callback, which is not meant to change attributes on the client
	const float CurrentHealth = GetHealth();
	const float EstimatedMagnitude = CurrentHealth - OldValue.GetCurrentValue();

	OnHealthChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);

	if (!bOutOfHealth && CurrentHealth <= 0.0f)
	{
		OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);
	}

	bOutOfHealth = (CurrentHealth <= 0.0f);
}

void UBotaniHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldValue);

	// Call the change callback, which is not meant to change attributes on the client
	OnMaxHealthChanged.Broadcast(nullptr, nullptr, nullptr, GetMaxHealth() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetMaxHealth());
}

void UBotaniHealthSet::OnRep_Shield(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Shield, OldValue);

	// Call the change callback, which is not meant to change attributes on the client
	const float CurrentShield = GetShield();
	const float EstimatedMagnitude = CurrentShield - OldValue.GetCurrentValue();

	OnShieldChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentShield);

	if (!bOutOfShield && CurrentShield <= 0.0f)
	{
		OnOutOfShield.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentShield);
	}

	bOutOfShield = (CurrentShield <= 0.0f);
}

void UBotaniHealthSet::OnRep_MaxShield(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxShield, OldValue);

	// Call the change callback, which is not meant to change attributes on the client
	OnMaxShieldChanged.Broadcast(nullptr, nullptr, nullptr, GetMaxShield() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetMaxShield());
}

bool UBotaniHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	// Handle modifying incoming normal damage
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(BotaniGameplayTags::Gameplay::Damage::TAG_GameplayDamage_SelfDestruct);

			if (Data.Target.HasMatchingGameplayTag(BotaniGameplayTags::Gameplay::Immune::TAG_GameplayImmune_Damage) && !bIsDamageFromSelfDestruct)
			{
				// Don't apply damage if the target is immune
				Data.EvaluatedData.Magnitude = 0.f;
				return false;
			}

#if !UE_BUILD_SHIPPING
			// Check for cheats
			if (Data.Target.HasMatchingGameplayTag(BotaniGameplayTags::Cheat::TAG_Cheat_GodMode)
				|| Data.Target.HasMatchingGameplayTag(BotaniGameplayTags::Cheat::TAG_Cheat_UnlimitedHealth))
			{
				// Don't apply damage if the target is in god mode or has unlimited health
				Data.EvaluatedData.Magnitude = 0.f;
				return false;
			}
#endif
		}
	}

	// Save the current state of the health
	MaxHealthBeforeAttributeChange = GetMaxHealth();
	HealthBeforeAttributeChange = GetHealth();
	MaxShieldBeforeAttributeChange = GetMaxShield();
	ShieldBeforeAttributeChange = GetShield();
	return true;
}

void UBotaniHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(BotaniGameplayTags::Gameplay::Damage::TAG_GameplayDamage_SelfDestruct);
	float MinimumHealth = 0.f;
	float MinimumShield = 0.f;

#if !UE_BUILD_SHIPPING
	// Check for cheats
	if (!bIsDamageFromSelfDestruct &&
		(Data.Target.HasMatchingGameplayTag(BotaniGameplayTags::Cheat::TAG_Cheat_GodMode)))
	{
		if (Data.Target.HasMatchingGameplayTag(BotaniGameplayTags::Cheat::TAG_Cheat_UnlimitedHealth))
		{
			MinimumHealth = 1.f;	
		}

		if (Data.Target.HasMatchingGameplayTag(BotaniGameplayTags::Cheat::TAG_Cheat_UnlimitedShield))
		{
			MinimumShield = 1.f;
		}
	}
#endif

	const FGameplayEffectContextHandle& EffectContextHandle = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContextHandle.GetOriginalInstigator();
	AActor* Causer = EffectContextHandle.GetEffectCauser();
	
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())								// Manage incoming damage
	{
		// Send a standard verb message that other systems can observe
		{
			FBotaniVerbMessage VerbMessage;
			VerbMessage.VerbTag = BotaniGameplayTags::Ability::Message::TAG_Damage_Message;
			VerbMessage.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
			VerbMessage.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
			VerbMessage.Target = GetOwningActor();
			VerbMessage.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
			VerbMessage.Magnitude = Data.EvaluatedData.Magnitude;

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(VerbMessage.VerbTag, VerbMessage);
		}
		
		if (!Data.EffectSpec.GetDynamicAssetTags().HasTagExact(BotaniGameplayTags::Gameplay::Damage::TAG_GameplayDamage_IgnoreShield) && GetShield() > 0.f)
		{
			if (Data.EffectSpec.GetDynamicAssetTags().HasTagExact(BotaniGameplayTags::Gameplay::Damage::TAG_GameplayDamage_BreakThroughShield))
			{
				SetDamage(FMath::Clamp((GetDamage() - GetShield()), 0.f, GetDamage()));
			}
			else
			{
				SetDamage(0.f);
			}
			
			// Convert damage into negative shield
			SetShield(FMath::Clamp(GetShield() - GetDamage(), MinimumShield, GetMaxShield()));
		}

		// Only subtract damage from health if the shield is depleted or the damage ignores the shield
		if (GetDamage() > 0.f)
		{
			// Convert damage into negative health
			SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
			SetDamage(0.f);	
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())							// Manage incoming healing
	{
		// Convert healing into positive health
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())						// Manage max health changes
	{
		// Notify on any requested max health changes
		OnMaxHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, GetMaxHealth() - MaxHealthBeforeAttributeChange, MaxHealthBeforeAttributeChange, GetMaxHealth());
	}



	// If health actually changed, notify the change
	if (GetHealth() != HealthBeforeAttributeChange)
	{
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, GetHealth() - HealthBeforeAttributeChange, HealthBeforeAttributeChange, GetHealth());
	}

	if ((GetHealth() <= 0.f) && !bOutOfHealth)
	{
		OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, GetHealth() - HealthBeforeAttributeChange, HealthBeforeAttributeChange, GetHealth());
	}

	// Check if the owner is dead now
	bOutOfHealth = (GetHealth() <= 0.f);


	
	// If shield actually changed, notify the change
	if (GetShield() != ShieldBeforeAttributeChange)
	{
		OnShieldChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, GetShield() - ShieldBeforeAttributeChange, ShieldBeforeAttributeChange, GetShield());
	}

	if ((GetShield() <= 0.f) && !bOutOfShield)
	{
		OnOutOfShield.Broadcast(Instigator, Causer, &Data.EffectSpec, GetShield() - ShieldBeforeAttributeChange, ShieldBeforeAttributeChange, GetShield());
	}

	// Check if the owner has no shield now
	bOutOfShield = (GetShield() <= 0.f);
}

void UBotaniHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UBotaniHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UBotaniHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	// Make sure the health is clamped and doesn't outreach the max health
	if (Attribute == GetMaxHealthAttribute())
	{
		if (GetHealth() > NewValue)
		{
			UBotaniAbilitySystemComponent* BotaniASC = GetBotaniAbilitySystemComponent();
			BotaniASC->ApplyModToAttribute(GetHealingAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	if (bOutOfHealth && GetHealth() > 0.f)
	{
		bOutOfHealth = false;
	}
}

void UBotaniHealthSet::ClampAttribute(const FGameplayAttribute& InAttribute, float& OutValue) const
{
	// Health should never be negative or later the max health
	if (InAttribute == GetHealthAttribute())
	{
		OutValue = FMath::Clamp(OutValue, 0.f, GetMaxHealth());
	}
	// Max health should never be below 1 as it would result in a division by zero
	if (InAttribute == GetMaxHealthAttribute())
	{
		OutValue = FMath::Max(OutValue, 1.f);
	}
}
