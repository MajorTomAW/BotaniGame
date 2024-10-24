// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Bot/Modifiers/CharacterModifier_Behavior.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterModifier_Behavior)

UCharacterModifier_Behavior::UCharacterModifier_Behavior(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	EditorFriendlyName = TEXT("Modifier: Behavior");
#endif

	bOverrideSightSenseSettings = 0;
	
	SightUnawareConfig = NewObject<UBotaniSenseSight_ConfigWrapper>();
	{
		SightUnawareConfig->SightRadius = 4000.f;
		SightUnawareConfig->LoseSightRadius = 4100.f;
		SightUnawareConfig->PeripheralVisionAngleDegrees = 30.f;
		SightUnawareConfig->MaxAge = 10.f;
	}
	
	SightSuspiciousConfig = NewObject<UBotaniSenseSight_ConfigWrapper>();
	{
		SightSuspiciousConfig->SightRadius = 8000.f;
		SightSuspiciousConfig->LoseSightRadius = 8500.f;
		SightSuspiciousConfig->PeripheralVisionAngleDegrees = 45.f;
		SightUnawareConfig->MaxAge = 10.f;
	}
	
	SightLostTargetConfig = NewObject<UBotaniSenseSight_ConfigWrapper>();
	{
		SightLostTargetConfig->SightRadius = 10000.f;
		SightLostTargetConfig->LoseSightRadius = 10500.f;
		SightLostTargetConfig->PeripheralVisionAngleDegrees = 30.f;
		SightUnawareConfig->MaxAge = 10.f;
	}
	
	SightThreatenedConfig = NewObject<UBotaniSenseSight_ConfigWrapper>();
	{
		SightThreatenedConfig->SightRadius = 12000.f;
		SightThreatenedConfig->LoseSightRadius = 12500.f;
		SightThreatenedConfig->PeripheralVisionAngleDegrees = 45.f;
		SightUnawareConfig->MaxAge = 10.f;
	}
}

void UCharacterModifier_Behavior::ApplyToCharacter(ACharacter* Character) const
{
	Super::ApplyToCharacter(Character);
}
