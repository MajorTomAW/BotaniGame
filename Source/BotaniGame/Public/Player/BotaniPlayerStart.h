// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerStart.h"
#include "BotaniPlayerStart.generated.h"

/**
 * EBotaniPlayerStartLocationOccupancy
 *
 * Enum to represent the occupancy of a player start location.
 */
namespace EBotaniPlayerStartLocationOccupancy
{
	enum Type : uint8
	{
		/** No player is occupying this location */
		Empty,

		/** A player is about to occupy or in the progress of occupying this location */
		Partial,

		/** A player is occupying this location */
		Full
	};
}

/**
 * ABotaniPlayerStart
 *
 * Base player start that it meant to be used in the game.
 */
UCLASS(Config = Game, Abstract, HideCategories = (Object, Rendering, Replication, Actor, Input, HLOD, Physics, Networking, LevelInstance, Cooking, DataLayers))
class BOTANIGAME_API ABotaniPlayerStart : public APlayerStart
{
	GENERATED_UCLASS_BODY()

public:
	const FGameplayTagContainer& GetOptGameplayTags() const { return OptStartPointTags; }
	const FGameplayTag& GetSpawnpointTag() const { return SpawnpointTag; }

	/** Returns true if this PlayerStart is already claimed by a controller */
	bool IsClaimed() const;

	/** Returns the priority of this player start */
	FORCEINLINE int32 GetPriority() const { return Priority; }

	/** Try to claim this PlayerStart for a controller, returns false if the PlayerStart is already claimed */
	bool TryClaim(AController* OccupyingController);

	/** Returns the occupancy of this PlayerStart location */
	EBotaniPlayerStartLocationOccupancy::Type GetLocationOccupancy(AController* const ControllerPawnToFit) const;

protected:
	/** The priority of this player start */
	UPROPERTY(EditAnywhere, Category = "Player Start", meta = (ClampMin = "0", UImin = "0"))
	int32 Priority;
	
	/** Tag to identify this player start */
	UPROPERTY(EditAnywhere, Category = "Player Start")
	FGameplayTag SpawnpointTag;
	
	/** Container of tags used to identify this player start */
	UPROPERTY(EditAnywhere, Category = "Player Start")
	FGameplayTagContainer OptStartPointTags;

	/** Whether this spawnpoint can be claimed by a player */
	UPROPERTY(EditAnywhere, Category = "Claiming")
	uint8 bIsClaimable:1;

	/** Interval in which we will check if this player start is not colliding with anyone anymore */
	UPROPERTY(EditAnywhere, Category = "Claiming", meta = (EditCondition = "bIsClaimable", ForceUnits = "seconds"))
	float ExpirationCheckInterval;

	/** Determines whether we respawn players that are alive when the Respawn at PlayerStart is called. */
	UPROPERTY(EditAnywhere, Category = "Respawning")
	uint8 bRespawnAlivePlayers:1;

#if WITH_EDITOR
protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	/** Checks if this PlayerStart is still claimed by a controller */
	void CheckUnclaimed();

	/** Handle to track expiration recurring timer */
	FTimerHandle ExpirationTimerHandle;

private:
	/** The controller that claimed this PlayerStart */
	UPROPERTY(Transient)
	TObjectPtr<AController> ClaimingController;
};
