// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniCharacterModifier.h"
#include "CharacterModifier_Cosmetic.generated.h"



/**
 * UCharacterModifier_Cosmetic
 *
 * Modifier that changes the cosmetic appearance of the character.
 * For example, changing the character's mesh, material, or color.
 */
UCLASS(meta = (DisplayName = "Cosmetic Modifier"))
class UCharacterModifier_Cosmetic : public UBotaniCharacterModifier
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void ApplyToCharacter(ACharacter* Character) const override;

protected:
	/** The character definition to apply to the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cosmetic")
	TSoftObjectPtr<class UBotaniCharacterDefinition> CharacterCosmetic;
};
