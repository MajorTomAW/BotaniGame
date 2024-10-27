// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/Instances/BotaniWeaponEquipmentInstance.h"

#include "Character/Components/Movement/BotaniMovementComponent.h"
#include "Inventory/Definitions/BotaniWeaponDefinition.h"
#include "Inventory/Weapons/Modes/WeaponMode_RangedWeapon.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniWeaponEquipmentInstance)

UBotaniWeaponEquipmentInstance::UBotaniWeaponEquipmentInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UBotaniWeaponEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ActiveModeTag, Params);
}

void UBotaniWeaponEquipmentInstance::OnEquipped(const FGameplayEquipmentSpec& EquipmentSpec, const FGameplayInventoryItemContext& Context)
{
	Super::OnEquipped(EquipmentSpec, Context);

	UWorld* World = GetWorld();
	check(World);
	TimeLastEquipped = World->GetTimeSeconds();
	JumpFallMultiplier = 1.f;

	// Activate the first weapon mode possible
	FGameplayTag FirstModeTag;
	
	UBotaniWeaponDefinition* WeaponDef = GetItemDefinition<UBotaniWeaponDefinition>();
	if (WeaponDef)
	{
		for (const auto& Mode : WeaponDef->WeaponModes)
		{
			if (Mode == nullptr || !Mode->WeaponModeTag.IsValid())
			{
				continue;
			}

			FirstModeTag = Mode->WeaponModeTag;
			break;
		}
	}

	// Try to activate the first mode
	if (FirstModeTag.IsValid())
	{
		ActivateWeaponMode(FirstModeTag);
	}
}

void UBotaniWeaponEquipmentInstance::OnUnequipped(const FGameplayEquipmentSpec& EquipmentSpec)
{
	Super::OnUnequipped(EquipmentSpec);

	UBotaniAbilitySystemComponent* BotaniASC = GetBotaniASC();
	if (BotaniASC == nullptr)
	{
		return;
	}
	
	// Clear the entire list of granted handles
	for (auto& KVP : GrantedModeHandles)
	{
		KVP.Value.TakeFromAbilitySystem(BotaniASC);
	}
}

bool UBotaniWeaponEquipmentInstance::ActivateWeaponMode(const FGameplayTag& NewModeTag)
{
	// Don't attempt to activate the same mode
	if (ActiveModeTag.MatchesTagExact(NewModeTag))
	{
		return false;
	}

	// Clean up the old mode
	DeactivateWeaponMode(ActiveModeTag);

	// Set the new mode
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ActiveModeTag, this);
	ActiveModeTag = NewModeTag;

	// Let the weapon mode know it was activated
	if (const UBotaniWeaponDefinition* WeaponDef = GetItemDefinition<UBotaniWeaponDefinition>())
	{
		APawn* Avatar = GetPawn();
		WeaponDef->ActivateWeaponMode(Avatar, NewModeTag, this);
	}

	return true;
}

void UBotaniWeaponEquipmentInstance::DeactivateWeaponMode(const FGameplayTag& OldModeTag)
{
	// Only deactivate if we have a valid mode
	if (!OldModeTag.IsValid())
	{
		return;
	}

	// Let the weapon mode know it was deactivated
	if (const UBotaniWeaponDefinition* WeaponDef = GetItemDefinition<UBotaniWeaponDefinition>())
	{
		APawn* Avatar = GetPawn();
		WeaponDef->DeactivateWeaponMode(Avatar, OldModeTag, this);
	}

	// Clear the granted handles for the mode
	if (FBotaniAbilitySet_GrantedHandles* Handles = GrantedModeHandles.Find(OldModeTag))
	{
		if (UBotaniAbilitySystemComponent* BotaniASC = GetBotaniASC())
		{
			Handles->TakeFromAbilitySystem(BotaniASC);
		}
	}
}

const UBotaniWeaponMode* UBotaniWeaponEquipmentInstance::GetActiveWeaponMode() const
{
	if (const UBotaniWeaponDefinition* WeaponDef = GetItemDefinition<UBotaniWeaponDefinition>())
	{
		return WeaponDef->FindWeaponMode(ActiveModeTag);
	}

	return nullptr;
}

void UBotaniWeaponEquipmentInstance::CacheModeSpecificHandles(const FGameplayTag& ModeTag, const FBotaniAbilitySet_GrantedHandles& GrantedHandles)
{
	GrantedModeHandles.Add(ModeTag, GrantedHandles);
}

void UBotaniWeaponEquipmentInstance::TickWeapon(float DeltaSeconds)
{
	APawn* Pawn = GetPawn();
	check(Pawn);

	UpdateSpread(DeltaSeconds);
	UpdateSpreadMultipliers(DeltaSeconds);
}

void UBotaniWeaponEquipmentInstance::OnRep_ActiveWeaponMode(FGameplayTag OldWeaponMode)
{
}

bool UBotaniWeaponEquipmentInstance::UpdateSpread(float DeltaSeconds)
{
	const float TimeSinceFired = GetWorld()->TimeSince(TimeLastFired);
	return false;
}

bool UBotaniWeaponEquipmentInstance::UpdateSpreadMultipliers(float DeltaSeconds)
{
	constexpr float NearlyEqualThresh = 0.04f;

	APawn* Pawn = GetPawn();
	check(Pawn);
	UBotaniMovementComponent* BotaniMovement = Cast<UBotaniMovementComponent>(Pawn->GetMovementComponent());
	
	FBotaniWeaponStatData Stats;
	const UBotaniWeaponDefinition* WeaponDef = GetItemDefinition<UBotaniWeaponDefinition>();
	if (!WeaponDef)
	{
		return false;
	}
	
	if (const UWeaponMode_RangedWeapon* Mode = WeaponDef->GetWeaponMode<UWeaponMode_RangedWeapon>())
	{
		Stats = *Mode->WeaponStats.GetRow<FBotaniWeaponStatData>(TEXT(""));
	}
	else
	{
		return false;
	}

	const float CurrentSpeed = Pawn->GetVelocity().Size();

	// See if we are in the air (jumping/falling), and if so, smoothly apply penalties
	const bool bIsJumpingOrFalling = BotaniMovement && BotaniMovement->IsFalling();
	const float JumpFallTargetValue = bIsJumpingOrFalling ? Stats.SpreadData.FallingSpreadMultiplier : 1.f;
	JumpFallMultiplier = FMath::FInterpTo<float>(JumpFallMultiplier, JumpFallTargetValue, DeltaSeconds, 5.f);
	const bool bJumpFallMultiplierIs1 = FMath::IsNearlyEqual(JumpFallMultiplier, 1.f, NearlyEqualThresh);

	// Update the spread multiplier and combine all multipliers
	CurrentSpreadMultiplier = JumpFallMultiplier;

	return bJumpFallMultiplierIs1;
}
