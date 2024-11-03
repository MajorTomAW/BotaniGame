// Copyright © 2024 Botanibots Team. All rights reserved.


#include "GameplayTags/BotaniGameplayTags.h"

#include "BotaniLogChannels.h"

namespace BotaniGameplayTags
{
	FGameplayTag FindTagByString(const FString& TagString, bool bMatchingPartialString)
	{
		const UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();
		FGameplayTag Tag = TagManager.RequestGameplayTag(FName(*TagString), bMatchingPartialString);

		if (!Tag.IsValid() && bMatchingPartialString)
		{
			FGameplayTagContainer AllTags;
			TagManager.RequestAllGameplayTags(AllTags, true);

			for (const FGameplayTag& TestTag : AllTags)
			{
				if (TestTag.ToString().Contains(TagString))
				{
					BOTANI_LOG(Display, TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."), *TagString, *TestTag.ToString());
					Tag = TestTag;
					break;
				}
			}
		}

		return Tag;
	}
}

////////////////////////////////////////////////////////////////////////////
/// Native Gameplay Tags 

/** Movement Mode Tags */
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Movement::TAG_MovementMode_Walking, "Movement.Mode.Walking", "Walking movement mode.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Movement::TAG_MovementMode_NavWalking, "Movement.Mode.NavWalking", "NavWalking movement mode.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Movement::TAG_MovementMode_Falling, "Movement.Mode.Falling", "Falling movement mode.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Movement::TAG_MovementMode_Swimming, "Movement.Mode.Swimming", "Swimming movement mode.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Movement::TAG_MovementMode_Flying, "Movement.Mode.Flying", "Flying movement mode.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Movement::TAG_MovementMode_Custom, "Movement.Mode.Custom", "Custom movement mode.");

/** Custom Movement Mode Tags */
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Movement::Custom::TAG_MovementMode_WallRunning, "Movement.Mode.Custom.WallRunning", "Wall running movement mode.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Movement::Custom::TAG_MovementMode_Sliding, "Movement.Mode.Custom.Sliding", "Sliding movement mode.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Movement::Custom::TAG_MovementMode_Custom_3, "Movement.Mode.Custom.3", "Custom movement mode 3.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Movement::Custom::TAG_MovementMode_Custom_4, "Movement.Mode.Custom.4", "Custom movement mode 4.");

/** Movement Modifier Tags */
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Movement::Modifier::TAG_MovementModifier_BlockWallRunning, "Movement.Modifier.Block.WallRun", "Blocks the character from wall running.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Movement::Modifier::TAG_MovementModifier_BlockWallJumping, "Movement.Modifier.Block.WallJump", "Blocks the character from wall jumping.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Movement::Modifier::TAG_MovementModifier_BlockSprinting, "Movement.Modifier.Block.Sprinting", "Blocks the character from sprinting.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Movement::Modifier::TAG_MovementModifier_BlockSliding, "Movement.Modifier.Block.Sliding", "Blocks the character from sliding.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Movement::Modifier::TAG_MovementModifier_BlockMoving, "Movement.Modifier.Block.Moving", "Blocks the character from moving.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Movement::Modifier::TAG_MovementModifier_BlockJumping, "Movement.Modifier.Block.Jumping", "Blocks the character from jumping.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Movement::Modifier::TAG_MovementModifier_BlockDashing, "Movement.Modifier.Block.Dashing", "Blocks the character from dashing.");


/** Input Tags */
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Input::TAG_InputTag, "Input.InputTag", "Input tag for generic input.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Input::TAG_InputTag_Move, "Input.InputTag.Move", "Input tag for movement.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Input::TAG_InputTag_Look_Mouse, "Input.InputTag.Look.Mouse", "Input tag for looking with the mouse.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Input::TAG_InputTag_Look_Stick, "Input.InputTag.Look.Stick", "Input tag for looking with the stick or gamepad.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Input::TAG_InputTag_Primary, "Input.InputTag.Primary", "Input tag for the primary mouse button.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Input::TAG_InputTag_Secondary, "Input.InputTag.Secondary", "Input tag for the secondary mouse button.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Input::TAG_InputTag_Interact, "Input.InputTag.Interact", "Input tag for the interact key.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Input::TAG_InputTag_DropItem, "Input.InputTag.DropItem", "Input tag for dropping the current item key.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Input::TAG_InputTag_Reload, "Input.InputTag.Reload", "Input tag for the reload key.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Input::TAG_InputTag_1, "Input.InputTag.1", "Input tag for the 1 key.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Input::TAG_InputTag_2, "Input.InputTag.2", "Input tag for the 2 key.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Input::TAG_InputTag_3, "Input.InputTag.3", "Input tag for the 3 key.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Input::TAG_InputTag_4, "Input.InputTag.4", "Input tag for the 4 key.");

/** Ability Tags */
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Ability::Restrict::TAG_AbilityRestrict_Weapon_PrimaryFire, "Ability.Restrict.Weapon.PrimaryFire", "Restricts the primary fire ability.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Ability::Restrict::TAG_AbilityRestrict_Weapon_SecondaryFire, "Ability.Restrict.Weapon.SecondaryFire", "Restricts the secondary fire ability.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Ability::Behavior::TAG_Ability_Behavior_SurvivesDeath, "Ability.Behavior.SurvivesDeath", "The ability survives the death of the actor.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Ability::ActivationFail::TAG_AbilityActivationFail_InvalidTarget, "Ability.ActivationFail.InvalidTarget", "The target of the ability is invalid.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Ability::ActivationFail::TAG_AbilityActivationFail_Cost, "Ability.ActivationFail.Cost", "The ability cannot be activated due to cost.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Ability::ActivationFail::TAG_AbilityActivationFail_Cooldown, "Ability.ActivationFail.Cooldown", "The ability cannot be activated due to cooldown.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Ability::ActivationFail::TAG_AbilityActivationFail_IsDead, "Ability.ActivationFail.IsDead", "The ability cannot be activated because the actor is dead.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Ability::ActivationFail::TAG_AbilityActivationFail_TagsBlocked, "Ability.ActivationFail.TagsBlocked", "The ability cannot be activated because the required tags are blocked.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Ability::ActivationFail::TAG_AbilityActivationFail_TagsMissing, "Ability.ActivationFail.TagsMissing", "The ability cannot be activated because the required tags are missing.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Ability::ActivationFail::TAG_AbilityActivationFail_FatalError, "Ability.ActivationFail.FatalError", "The ability cannot be activated due to a fatal error.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Ability::ActivationFail::TAG_AbilityActivationFail_MagazineFull, "Ability.ActivationFail.MagazineFull", "The ability cannot be activated because the magazine is full.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Ability::ActivationFail::TAG_AbilityActivationFail_OutOfAmmo, "Ability.ActivationFail.OutOfAmmo", "The ability cannot be activated because the actor is out of ammo.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Ability::Message::TAG_AbilityActivationFail_Message, "Ability.ActivationFail.Message", "The message to display when an ability activation fails.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Ability::Message::TAG_Damage_Message, "Ability.Message.Damage", "The message to display when damage is dealt.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Ability::Message::TAG_Healing_Message, "Ability.Message.Healing", "The message to display when healing is applied.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Ability::Message::TAG_Ammo_Message, "Ability.Message.Ammo", "The message to display when ammo is used.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Ability::Descriptor::Action::TAG_AbilityAction_Reload, "Ability.Descriptor.Action.Reload", "The action to reload the weapon.");

/** Effect Tags */
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Effect::SetByCaller::TAG_SetByCaller_Damage, "GameplayEffect.SetByCaller.Damage", "SetByCaller tag used by damage gameplay effects.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Effect::SetByCaller::TAG_SetByCaller_Heal, "GameplayEffect.SetByCaller.Heal", "SetByCaller tag used by healing gameplay effects.");

/** Init State Tags */
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::InitState::TAG_InitState_Spawned, "InitState.Spawned", "1: Actor/component has initially spawned and can be extended.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::InitState::TAG_InitState_DataAvailable, "InitState.DataAvailable", "2: All required data has been loaded/replicated and is ready for initialization.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::InitState::TAG_InitState_DataInitialized, "InitState.DataInitialized", "3: The available data has been initialized for this actor/component, but it is not ready for full gameplay.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::InitState::TAG_InitState_GameplayReady, "InitState.GameplayReady", "4: The actor/component is fully ready for active gameplay.");

/** Cheat Tags */
#if !UE_BUILD_SHIPPING
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Cheat::TAG_Cheat_GodMode, "Cheat.GodMode", "The actor is invincible, can fly, and has unlimited resources.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Cheat::TAG_Cheat_Fly, "Cheat.Fly", "The actor can fly.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Cheat::TAG_Cheat_UnlimitedHealth, "Cheat.UnlimitedHealth", "The actor has unlimited health.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Cheat::TAG_Cheat_UnlimitedAmmo, "Cheat.UnlimitedMana", "The actor has unlimited ammo.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Cheat::TAG_Cheat_UnlimitedShield, "Cheat.UnlimitedShield", "The actor has unlimited shield.");
#endif

/** Status Tags */
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Status::TAG_Death, "Status.Death", "The actor is dead.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Status::TAG_Death_Dying, "Status.Death.Dying", "The actor is in the process of dying very slowly and painfully hehe...");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Status::TAG_Death_Dead, "Status.Death.Dead", "The actor is very dead now and has finished dying.");

/** Gameplay Event Tags */
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Gameplay::Event::TAG_GameplayEvent_Death, "Gameplay.Event.Death", "Event that fires on death. This event only fires on the server.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Gameplay::Event::TAG_GameplayEvent_OutOfAmmo, "Gameplay.Event.OutOfAmmo", "Event that fires on out of ammo. This event only fires on the server.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Gameplay::Event::TAG_GameplayEvent_Reset, "Gameplay.Event.Reset", "Event that fires on reset. This event only fires on the server.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Gameplay::Event::TAG_GameplayEvent_RequestReset, "Gameplay.Event.RequestReset", "Event that fires on request reset. This event only fires on the server.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Gameplay::Verb::TAG_GameplayVerb_Elimination, "Gameplay.Verb.Elimination", "Verb message that fires on elimination.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Gameplay::TAG_Gameplay_AbilityInputBlocked, "Gameplay.AbilityInputBlocked", "Tag to block ability input.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Gameplay::Damage::TAG_GameplayDamage_SelfDestruct, "GameplayEffect.Damage.SelfDestruct", "Tag for falling out of the world aka self destructing.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Gameplay::Damage::TAG_GameplayDamage_FellOutOfWorld, "GameplayEffect.Damage.FellOutOfWorld", "Tag for falling out of the world.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Gameplay::Damage::TAG_GameplayDamage_IgnoreShield, "GameplayEffect.Damage.IgnoreShield", "Tag for applying damage that ignores shields.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Gameplay::Damage::TAG_GameplayDamage_AllowFriendlyFire, "GameplayEffect.Damage.AllowFriendlyFire", "Tag for allowing friendly fire so damage can be applied to friendly actors.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Gameplay::Damage::TAG_GameplayDamage_BreakThroughShield, "GameplayEffect.Damage.BreakThroughShield", "Tag for breaking through shields. Meaning the damage will be applied to health if the shield is depleted.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Gameplay::Immune::TAG_GameplayImmune_Damage, "GameplayEffect.Immune.Damage", "Tag for immunity to damage.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Gameplay::Message::TAG_GameplayMessage, "Gameplay.Message", "Tag for generic gameplay message.");

/** UI Tags */
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::UI::TAG_UI_Action_Escape, "UI.Action.Escape", "Tag for the escape action.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::UI::TAG_UI_Layer_Menu, "UI.Layer.Menu", "Tag for the menu layer.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::UI::TAG_UI_Layer_Game, "UI.Layer.Game", "Tag for the game layer.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::UI::TAG_UI_Layer_Modal, "UI.Layer.Modal", "Tag for the modal layer.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::UI::TAG_UI_Layer_Settings, "UI.Layer.Settings", "Tag for the settings layer.");

/** Item Tags */
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Item::Stat::TAG_Item_Stat_Durability, "Item.Stat.Durability", "Tag for durability stat of an item.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Item::Weapon::Mode::TAG_WeaponMode_Melee, "Item.Weapon.Mode.Melee", "Tag for melee weapon mode.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Item::Weapon::Mode::TAG_WeaponMode_Ranged, "Item.Weapon.Mode.Ranged", "Tag for ranged weapon mode.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Item::Weapon::Stat::TAG_WeaponStat_MagazineAmmo, "Item.Weapon.Stat.MagazineAmmo", "Tag for magazine ammo stat of a weapon.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Item::Weapon::Stat::TAG_WeaponStat_MagazineSize, "Item.Weapon.Stat.MagazineSize", "Tag for magazine size stat of a weapon.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Item::Weapon::Stat::TAG_WeaponStat_SpareAmmo, "Item.Weapon.Stat.SpareAmmo", "Tag for spare ammo stat of a weapon.");

/** Inventory Tags */
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Inventory::Message::TAG_InventoryMessage_QuickBar_SlotsChanged, "Inventory.Message.QuickBar.SlotsChanged", "Message for when the quick bar slots have changed.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(BotaniGameplayTags::Inventory::Message::TAG_InventoryMessage_QuickBar_ActiveIndexChanged, "Inventory.Message.QuickBar.ActiveIndexChanged", "Message for when the active index of the quick bar has changed.");