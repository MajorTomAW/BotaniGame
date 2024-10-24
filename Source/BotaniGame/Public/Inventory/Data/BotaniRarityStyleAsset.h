// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Inventory/Definitions/BotaniInventoryItemDefinition.h"
#include "Slate/SlateBrushAsset.h"
#include "BotaniRarityStyleAsset.generated.h"

USTRUCT(BlueprintType)
struct FBotaniRarityStyleInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rarity")
	TMap<FName, float> ScalarParameters;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rarity")
	TMap<FName, FLinearColor> ColorParameters;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rarity")
	TMap<FName, TObjectPtr<UTexture2D>> TextureParameters;
};

/**
 * UBotaniRarityStyleAsset
 */
UCLASS(BlueprintType)
class BOTANIGAME_API UBotaniRarityStyleAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/** returns the rarity style for the specified rarity */
	UFUNCTION(BlueprintCallable, Category = "Botani|Rarity")
	FBotaniRarityStyleInfo GetRarityStyle(const EBotaniItemRarity Rarity) const
	{
		if (RarityStyles.Contains(Rarity))
		{
			return RarityStyles[Rarity];	
		}

		return FBotaniRarityStyleInfo();
	}
	
	/** Returns the rarity style for the specified rarity (as a color): */
	UFUNCTION(BlueprintCallable, Category = "Botani|Rarity", meta = (DisplayName = "GetRarityStyle (Color)"))
	FLinearColor GetRarityStyleColor(const EBotaniItemRarity Rarity, const FName Param, bool& OutSuccess) const;

	/** Returns the rarity style for the specified rarity (as a scalar): */
	UFUNCTION(BlueprintCallable, Category = "Botani|Rarity", meta = (DisplayName = "GetRarityStyle (Scalar)"))
	float GetRarityStyleScalar(const EBotaniItemRarity Rarity, const FName Param, bool& OutSuccess) const;

	/** Returns the rarity style for the specified rarity (as a texture): */
	UFUNCTION(BlueprintCallable, Category = "Botani|Rarity", meta = (DisplayName = "GetRarityStyle (Texture)"))
	UTexture2D* GetRarityStyleTexture(const EBotaniItemRarity Rarity, const FName Param, bool& OutSuccess) const;
	
	UTexture2D* GetDefaultIcon() const
	{
		return DefaultIcon.Get();
	}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rarity")
	TObjectPtr<UTexture2D> DefaultIcon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rarity")
	TMap<EBotaniItemRarity, FBotaniRarityStyleInfo> RarityStyles;
};
