// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Bot/Modifiers/CharacterModifier_Cosmetic.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterModifier_Cosmetic)

UCharacterModifier_Cosmetic::UCharacterModifier_Cosmetic(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	EditorFriendlyName = TEXT("Modifier: Cosmetic");
#endif
}

void UCharacterModifier_Cosmetic::ApplyToCharacter(ACharacter* Character) const
{
	Super::ApplyToCharacter(Character);
}
