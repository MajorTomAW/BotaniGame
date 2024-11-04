// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "BotaniMovementTypes.generated.h"

UENUM()
enum EBotaniWallRunSide
{
	Wall_Left = 1 << 0,
	Wall_Right = 1 << 1,
	Wall_Error = 0,
	Wall_Both = Wall_Left | Wall_Right
};

/**
 * FBotaniGrappleRopeConfig
 *
 * Configuration for a grapple rope.
 */
USTRUCT(BlueprintType)
struct FBotaniGrappleRopeConfig
{
	GENERATED_BODY()

public:
	/** Determines the virtual length of the grapple rope. Useful for allowing the character to stay grappling even if he is further away than the Max Distance. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rope", meta = (UIMin = 0, ClampMin = 0, Units = "cm", DisplayName = "Grapple Rope Length"))
	float Grapple_RopeLength = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rope")
	uint32 bGrapple_AlwaysUseFullLength : 1 = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rope")
	uint32 bGrapple_BreakRopeOnMaxDistance : 1 = true;

	/** The elasticity of the grapple rope. 0% means the rope is rigid, 100% means the rope is fully made out of rubber. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rope", meta = (UIMin = 0, ClampMin = 0, UIMax= 100, ClampMax = 100, DisplayName = "Grapple Rope Elasticity", Units = "%"))
	float Grapple_RopeElasticity = 0.f;

	/** The pulling factory applied to the default pulling force. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rope", meta = (UIMin = 0, ClampMin = 0, DisplayName = "Grapple Rope Pulling Factor"))
	float Grapple_RopePullingFactor = 1.f;
};

/**
 * EBotaniCustomMovementMode
 *
 * Custom movement modes for Botani characters.
 */
UENUM(BlueprintType)
enum EBotaniCustomMovementMode
{
	/** No special movement mode. */
	CMOVE_None			UMETA(Hidden),

	/** Movement mode for sliding along surfaces. */
	CMOVE_Slide			UMETA(DisplayName = "Slide"),
	
	/** Movement mode for running on walls. */
	CMOVE_WallRun		UMETA(DisplayName = "Wall Run"),

	/** Movement mode for the grappling hook. */
	CMOVE_Grapple		UMETA(DisplayName = "Grapple"),

	CMOVE_Max			UMETA(Hidden)
};

//////////////////////////////////////////////////////////////////////////
/// FSavedMove_BotaniCharacter

class FSavedMove_BotaniCharacter : public FSavedMove_Character
{
public:
	FSavedMove_BotaniCharacter();

	// Botani character specific flags.
	enum CompressedFlags
	{
		FLAG_WantsToSlide		= 0x10, //@TODO: this might be redundant if we use crouching for sliding
		FLAG_WantsToGrapple		= 0x20,
		FLAG_Custom_2			= 0x40,
		FLAG_Custom_3			= 0x80,
	};
	
public:
	/** Flags */
	uint8 bWantsToSlide		: 1;
	uint8 bWantsToGrapple	: 1;

	/** @note Non-safe flag. */
	uint8 bWantsToWallRun	: 1;
	uint8 bPrevWantsToCrouch : 1;

public:
	//~ Begin FSavedMove_Character Interface
	virtual void Clear() override;
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, const FVector& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
	virtual void PrepMoveFor(ACharacter* Character) override;
	virtual uint8 GetCompressedFlags() const override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
	//~ End FSavedMove_Character Interface	
};

//////////////////////////////////////////////////////////////////////////
/// FNetworkPredictionData_Client_BotaniCharacter

class FNetworkPredictionData_Client_BotaniCharacter : public FNetworkPredictionData_Client_Character
{
	using Super = FNetworkPredictionData_Client_Character;
	
public:
	FNetworkPredictionData_Client_BotaniCharacter(const UCharacterMovementComponent& ClientMovement);

	//~ Begin FNetworkPredictionData_Client_Character Interface
	virtual FSavedMovePtr AllocateNewMove() override;
	//~ End FNetworkPredictionData_Client_Character Interface
};