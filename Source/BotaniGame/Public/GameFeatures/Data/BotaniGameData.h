// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BotaniGameData.generated.h"

class UGameplayEffect;

/**
 * UBotaniGameData
 *
 * Non-mutable data asset that contains global game data.
 */
UCLASS(BlueprintType, Const, meta = (DisplayName = "Botani Game Data", ShortTooltip = "Data asset containing global game data."))
class BOTANIGAME_API UBotaniGameData : public UPrimaryDataAsset
{
	GENERATED_UCLASS_BODY()

public:
	/** Static getter for the loaded game data. */
	static const UBotaniGameData& Get();

public:
#pragma region gameplay_effects
	/** Gameplay effect used to apply damage. Uses SetByCaller for the damage magnitude. */
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects", meta = (DisplayName = "Damage Gameplay Effect (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> DamageGameplayEffect_SetByCaller;

	/** Gameplay effect used to apply healing. Uses SetByCaller for the healing magnitude. */
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects", meta = (DisplayName = "Heal Gameplay Effect (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> HealGameplayEffect_SetByCaller;

	/** Gameplay effect used to add and remove dynamic tags. */
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects")
	TSoftClassPtr<UGameplayEffect> DynamicTagGameplayEffect;
#pragma endregion

#pragma region gameplay_attribute
	/** Data asset containing info about attributes and their backing classes. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Gameplay Attributes", meta = (DisplayName = "Backing Attribute Data"))
	TSoftObjectPtr<class UBotaniBackingAttributeData> BotaniBackingAttributeData;
#pragma endregion
	
#pragma region data
	/** Data asset containing death cause data used to determine the cause of death. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data Lookup", meta = (DisplayName = "Death Cause Data"))
	TSoftObjectPtr<class UBotaniDeathCauseData> BotaniDeathCauseData;
	
	/** Lookup table for all attributes in the game. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data Lookup", meta = (DisplayName = "Attribute Lookup Table", RequiredAssetDataTags = "RowStructure=/Script/BotaniGame.BotaniAttributeLookupTableRow"))
	TSoftObjectPtr<class UCompositeDataTable> BotaniAttributeLookupTable;
	
	/** Data asset containing data about gameplay tags. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Gameplay Tags", meta = (DisplayName = "Gameplay Tag Data"))
	TSoftObjectPtr<class UBotaniGameplayTagData> BotaniGameplayTagData;

	/** Data asset containing data about rarity. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data Lookup", meta = (DisplayName = "Rarity Data"))
	TSoftObjectPtr<class UBotaniRarityStyleAsset> BotaniRarityData;
#pragma endregion

	//@TODO: Eventuell eine beseere Lösung finden die einzelnen assets abzugreifen
};
