// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "NativeGameplayTags.h"
#include "Character/Components/Movement/BotaniMovementTypes.h"


namespace BotaniGameplayTags
{
	BOTANIGAME_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchingPartialString = false);

	namespace Movement
	{
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MovementMode_Walking);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MovementMode_NavWalking);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MovementMode_Falling);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MovementMode_Swimming);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MovementMode_Flying);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MovementMode_Custom);

		namespace Custom
		{
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MovementMode_WallRunning);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MovementMode_Sliding);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MovementMode_Custom_3);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MovementMode_Custom_4);

			// Custom Movement Modes
			const TMap<uint8, FGameplayTag> CustomMovementModeTagMap =
			{
				{CMOVE_WallRun, TAG_MovementMode_WallRunning},
				{CMOVE_Slide, TAG_MovementMode_Sliding},
				{3, TAG_MovementMode_Custom_3},
				{4, TAG_MovementMode_Custom_4}
			};
		}

		// Unreal Engine Movement Modes
		const TMap<uint8, FGameplayTag> MovementModeTagMap =
		{
			{MOVE_Walking, TAG_MovementMode_Walking},
			{MOVE_NavWalking, TAG_MovementMode_NavWalking},
			{MOVE_Falling, TAG_MovementMode_Falling},
			{MOVE_Swimming, TAG_MovementMode_Swimming},
			{MOVE_Flying, TAG_MovementMode_Flying},
			{MOVE_Custom, TAG_MovementMode_Custom}
		};

		namespace Modifier
		{
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MovementModifier_BlockWallRunning);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MovementModifier_BlockWallJumping);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MovementModifier_BlockSprinting);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MovementModifier_BlockSliding);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MovementModifier_BlockMoving);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MovementModifier_BlockJumping);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MovementModifier_BlockDashing);
		}
	}

	namespace Input
	{
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag_Move);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag_Look_Mouse);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag_Look_Stick);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag_Primary);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag_Secondary);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag_Interact);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag_DropItem);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag_Reload);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag_1);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag_2);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag_3);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag_4);
	}

	namespace Ability
	{
		namespace Restrict
		{
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AbilityRestrict_Weapon_PrimaryFire);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AbilityRestrict_Weapon_SecondaryFire);
		}
		
		namespace Behavior
		{
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Behavior_SurvivesDeath);
		}

		namespace ActivationFail
		{
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AbilityActivationFail_InvalidTarget);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AbilityActivationFail_Cost);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AbilityActivationFail_Cooldown);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AbilityActivationFail_IsDead);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AbilityActivationFail_TagsBlocked);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AbilityActivationFail_TagsMissing);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AbilityActivationFail_FatalError);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AbilityActivationFail_MagazineFull);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AbilityActivationFail_OutOfAmmo);
		}

		namespace Message
		{
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AbilityActivationFail_Message);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Damage_Message);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Healing_Message);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ammo_Message);
		}

		namespace Descriptor
		{
			namespace Action
			{
				BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AbilityAction_Reload);
			}
		}
	}

	namespace Effect
	{
		namespace SetByCaller
		{
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_SetByCaller_Damage);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_SetByCaller_Heal);
		}
	}

	namespace InitState
	{
		/** 1: Actor/component has initially spawned and can be extended */
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InitState_Spawned);
		/** 2: All required data has been loaded/replicated and is ready for initialization */
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InitState_DataAvailable);
		/** 3: The available data has been initialized for this actor/component, but it is not ready for full gameplay */
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InitState_DataInitialized);
		/** 4: The actor/component is fully ready for active gameplay */
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InitState_GameplayReady);
	}
	
#if !UE_BUILD_SHIPPING
	namespace Cheat
	{
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Cheat_GodMode);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Cheat_Fly);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Cheat_UnlimitedHealth);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Cheat_UnlimitedAmmo);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Cheat_UnlimitedShield);
	}
#endif

	namespace Status
	{
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Death);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Death_Dying);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Death_Dead);
	}

	namespace Gameplay
	{
		namespace Event
		{
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayEvent_Death);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayEvent_Reset);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayEvent_RequestReset);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayEvent_OutOfAmmo);
		}

		namespace Verb
		{
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayVerb_Elimination);
		}

		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_AbilityInputBlocked);

		namespace Damage
		{
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayDamage_SelfDestruct);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayDamage_FellOutOfWorld);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayDamage_IgnoreShield);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayDamage_AllowFriendlyFire);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayDamage_BreakThroughShield);
		}

		namespace Immune
		{
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayImmune_Damage); //@TODO: macht mehr sinn das als GameplayEffect zu haben
		}

		namespace Message
		{
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayMessage);
		}
	}

	namespace UI
	{
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Action_Escape);
		
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Layer_Menu);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Layer_Game);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Layer_Modal);
		BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Layer_Settings);
	}

	namespace Item
	{
		namespace Stat
		{
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Stat_Durability);
		}

		namespace Weapon
		{
			namespace Mode
			{
				BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WeaponMode_Ranged);
				BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WeaponMode_Melee);
			}

			namespace Stat
			{
				BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WeaponStat_MagazineAmmo);
				BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WeaponStat_MagazineSize);
				BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WeaponStat_SpareAmmo);
			}
		}
	}

	namespace Inventory
	{
		namespace Message
		{
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InventoryMessage_QuickBar_SlotsChanged);
			BOTANIGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InventoryMessage_QuickBar_ActiveIndexChanged);
		}
	}
}
