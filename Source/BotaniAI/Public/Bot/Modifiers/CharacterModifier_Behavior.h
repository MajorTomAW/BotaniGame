// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniCharacterModifier.h"
#include "Perception/AISenseConfig_Sight.h"
#include "CharacterModifier_Behavior.generated.h"

UCLASS(EditInlineNew, meta = (DisplayName = "AI Sight Config"))
class UBotaniSenseSight_ConfigWrapper : public UObject
{
	GENERATED_BODY()

public:
	/** The sight radius of the sense. */
	UPROPERTY(EditDefaultsOnly, Category = "Sight", meta = (Units = "cm"))
	float SightRadius = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Sight", meta = (Units = "cm"))
	float LoseSightRadius = 3500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Sight", meta = (Units = "degrees", ClampMin = "0.0", ClampMax = "1800.0", UIMin = "0.0", UIMax = "180.0"))
	float PeripheralVisionAngleDegrees = 45.f;

	UPROPERTY(EditDefaultsOnly, Category = "Sight")
	FAISenseAffiliationFilter DetectionByAffiliation = FAISenseAffiliationFilter(1,0,0);

	UPROPERTY(EditDefaultsOnly, Category = "Sight", meta = (Units = "s"))
	float MaxAge = 10.f;
};

/**
 * UCharacterModifier_Behavior
 *
 * Modifier that changes the behavior of the character.
 */
UCLASS(meta = (DisplayName = "Behavior Modifier"))
class UCharacterModifier_Behavior : public UBotaniCharacterModifier
{
	GENERATED_UCLASS_BODY()

public:
	virtual void ApplyToCharacter(ACharacter* Character) const override;

protected:
	/** Whether to override the sight sense settings. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	uint32 bOverrideSightSenseSettings : 1;

	/** The unaware state for the sight sense. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Perception|Sight", meta = (EditCondition = "bOverrideSightSenseSettings", DisplayName = "Unaware"))
	TObjectPtr<UBotaniSenseSight_ConfigWrapper> SightUnawareConfig;

	/** The suspicious state for the sight sense. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Perception|Sight", meta = (EditCondition = "bOverrideSightSenseSettings", DisplayName = "Suspicious"))
	TObjectPtr<UBotaniSenseSight_ConfigWrapper> SightSuspiciousConfig;

	/** The lost target state for the sight sense. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Perception|Sight", meta = (EditCondition = "bOverrideSightSenseSettings", DisplayName = "Lost Target"))
	TObjectPtr<UBotaniSenseSight_ConfigWrapper> SightLostTargetConfig;

	/** The threatened state for the sight sense. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced ,Category = "Perception|Sight", meta = (EditCondition = "bOverrideSightSenseSettings", DisplayName = "Threatened"))
	TObjectPtr<UBotaniSenseSight_ConfigWrapper> SightThreatenedConfig;


	/** The behavior tree to apply to the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
	TSoftObjectPtr<class UBehaviorTree> BehaviorTree;
};
