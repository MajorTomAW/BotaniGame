// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniCharacterModifier.h"
#include "CharacterModifier_Movement.generated.h"

/**
 * FCharacterMovementModifier_Speeds
 *
 * Describes the movement speeds of the character.
 */
USTRUCT(BlueprintType)
struct FCharacterMovementModifier_Speeds
{
	GENERATED_BODY()

	FCharacterMovementModifier_Speeds()
		: WalkSpeed(1.5f)
		, RunSpeed(6.f)
		, Acceleration(20.48f)
		, Deceleration(20.48f)
	{
	}

public:
	/** The maximum ground speed when walking. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speeds", meta = (ClampMin = "0.0", Units = "m/s"))
	float WalkSpeed;

	/** The maximum ground speed when running. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speeds", meta = (ClampMin = "0.0", Units = "m/s"))
	float RunSpeed;

	/** The acceleration when moving on ground. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speeds", meta = (ClampMin = "0.0", Units = "m/s^2"))
	float Acceleration;

	/** The deceleration when moving on ground. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speeds", meta = (ClampMin = "0.0", Units = "m/s^2"))
	float Deceleration;

public:
	/** Applies the movement speeds to the movement component. */
	void ApplyToMovementComponent(class UCharacterMovementComponent* MovementComponent) const;
};

/**
 * FCharacterMovementModifier_World
 *
 * Describes the world properties of the character.
 */
USTRUCT(BlueprintType)
struct FCharacterMovementModifier_World
{
	GENERATED_BODY()

	FCharacterMovementModifier_World()
		: GravityScale(1.f)
	{
	}

public:
	/** The gravity scale of the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World", meta = (ClampMin = "0.0", Units = "x"))
	float GravityScale;

public:
	/** Applies the world properties to the movement component. */
	void ApplyToMovementComponent(class UCharacterMovementComponent* MovementComponent) const;
};

/**
 * UCharacterModifier_Movement
 *
 * Modifies movement properties of the character.
 */
UCLASS(meta = (DisplayName = "Movement Modifier"))
class UCharacterModifier_Movement : public UBotaniCharacterModifier
{
	GENERATED_UCLASS_BODY()

public:
	virtual void ApplyToCharacter(ACharacter* Character) const override;

protected:
	/** The world properties of the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	FCharacterMovementModifier_World World;
	
	/** The movement speeds of the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FCharacterMovementModifier_Speeds Speeds;
};
