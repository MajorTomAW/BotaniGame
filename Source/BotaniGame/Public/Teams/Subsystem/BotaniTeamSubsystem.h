// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BotaniTeamSubsystem.generated.h"

#define BOTANI_TEAM_GLOBAL_ID_BOT 64
#define BOTANI_TEAM_GLOBAL_ID_PLAYER 128

struct FGameplayTag;
/** A delegate for when the display asset for a team changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBotaniTeamDisplayAssetChangedDelegate, const class UBotaniTeamDisplayAsset*, DisplayAsset);

//////////////////////////////////////////////////////////////////////////
/// Botani Team Types
//////////////////////////////////////////////////////////////////////////

/**
 * FBotaniTeamTrackingInfo
 *
 * Struct for tracking team information such as public and private team info, and the display asset
 */
USTRUCT(BlueprintType)
struct FBotaniTeamTrackingInfo
{
	GENERATED_BODY()

public:
	/** The public team info for the team */
	UPROPERTY()
	TObjectPtr<class ABotaniTeamPublicInfo> PublicInfo = nullptr;

	/** The private team info for the team */
	UPROPERTY()
	TObjectPtr<class ABotaniTeamPrivateInfo> PrivateInfo = nullptr;
	
	/** The display asset for the team */
	UPROPERTY()
	TObjectPtr<class UBotaniTeamDisplayAsset> DisplayAsset = nullptr;

	/** Delegate for when the display asset changes */
	UPROPERTY()
	FOnBotaniTeamDisplayAssetChangedDelegate OnTeamDisplayAssetChanged;

public:
	void SetTeamInfo(class ABotaniTeamInfoBase* Info);
	void RemoveTeamInfo(class ABotaniTeamInfoBase* Info);
};

/**
 * EBotaniTeamComparison
 *
 * Result of comparing the team affiliation for two actors
 */
UENUM(BlueprintType)
enum class EBotaniTeamComparison : uint8
{
	/** Both actors are members of the same team */
	OnSameTeam,

	/** The actors are members of opposing teams */
	DifferentTeams,

	/** One (or both) of the actors was invalid or not part of any team */
	InvalidArgument
};

//////////////////////////////////////////////////////////////////////////
/// BotaniTeamSubsystem
//////////////////////////////////////////////////////////////////////////

/**
 * UBotaniTeamSubsystem
 *
 * A subsystem for easy access to team information for team-based actors (for example, pawns or player states)
 */
UCLASS(meta = (DisplayName = "Team Sub (Botani)"))
class BOTANIGAME_API UBotaniTeamSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UBotaniTeamSubsystem();

	//~ Begin UWorldSubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~ End UWorldSubsystem Interface

public:
	/** Tries to register a new team */
	bool RegisterTeamInfo(class ABotaniTeamInfoBase* TeamInfo);

	/** Tries to unregister a team */
	bool UnregisterTeamInfo(class ABotaniTeamInfoBase* TeamInfo);

	/** Changes the team associated with this actor if possible, only callable on the authority */
	static bool ChangeTeamForActor(class AActor* Actor, int32 NewTeamId);

	/** Returns the team this object belongs to, or INDEX_NONE if it is not part of a team */
	static int32 FindTeamFromObject(const UObject* Object);

	/** Returns the associated player state for this actor, may return nullptr if it is not associated with a player */
	static const class ABotaniPlayerState* FindPlayerStateForActor(const AActor* Actor);

	/** Called when a team display asset has been edited, causes all team observers to update */
	void NotifyTeamDisplayAssetChanged(const UBotaniTeamDisplayAsset* DisplayAsset);

	/** Register for a team display asset notification for the specified team */
	FOnBotaniTeamDisplayAssetChangedDelegate& GetTeamDisplayAssetChangedDelegate(int32 TeamId);

	/** Returns the global team ID for bots */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Teams", meta = (Keywords = "Get, Team, Id, Bot, Enemy", CompactNodeTitle = "Bot Team ID"))
	static int32 GetGlobalBotTeamID() { return BOTANI_TEAM_GLOBAL_ID_BOT; }

	/** Returns the global team ID for players */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Teams", meta = (Keywords = "Get, Team, Id, Player", CompactNodeTitle = "Player Team ID"))
	static int32 GetGlobalPlayerTeamID() { return BOTANI_TEAM_GLOBAL_ID_PLAYER; }
	
	/** Returns the team this actor belongs to, or INDEX_NONE if it is not part of a team */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Teams", meta = (Keywords = "Get"))
	void FindTeamFromActor(const AActor* TestActor, bool& bIsPartOfTeam, int32& TeamId) const;

	/** Compare the teams of two actors and returns a value if they're on the same team, different teams, or one/both are invalid */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Botani|Teams", meta = (Keywords = "Get", ExpandEnumAsExecs = ReturnValue))
	static EBotaniTeamComparison CompareTeams(const AActor* ActorA, const AActor* ActorB, int32& TeamA, int32& TeamB);

	/** Compare the teams of two actors and returns a value if they're on the same team, different teams, or one/both are invalid */
	static EBotaniTeamComparison CompareTeams(const AActor* ActorA, const AActor* ActorB);

	/** Returns true if the instigator can damage the target, taking into account the friendly fire settings */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Teams")
	static bool CanCauseDamage(const UObject* Instigator, const UObject* Target, bool bAllowDamageToSelf = true);

	/** Adds a specified number of stacks to the tag (does nothing if StackCount is below 1) */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Botani|Teams")
	void AddTeamTagStack(int32 TeamId, const FGameplayTag& Tag, int32 StackCount);

	/** Removes a specified number of stacks from the tag (does nothing if StackCount is below 1) */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Botani|Teams")
	void RemoveTeamTagStack(int32 TeamId, const FGameplayTag& Tag, int32 StackCount);

	/** Returns the stack count of the specified tag (or 0 if the tag is not present) */
	UFUNCTION(BlueprintCallable, Category = "Botani|Teams")
	int32 GetTeamTagStackCount(int32 TeamId, const FGameplayTag& Tag) const;

	/** Returns true if there is at least one stack of the specified tag */
	UFUNCTION(BlueprintCallable, Category = "Botani|Teams")
	bool TeamHasTag(int32 TeamId, const FGameplayTag& Tag) const;

	/** Returns true if the specified team exists */
	UFUNCTION(BlueprintCallable, Category = "Botani|Teams")
	bool DoesTeamExist(int32 TeamId) const;

	/** Returns the list of teams */
	UFUNCTION(BlueprintCallable, Category = "Botani|Teams")
	TArray<int32> GetTeamIDs() const;

	/**
	 * Gets the team display asset for the specified team, from the perspective of the specified team.
	 * (You have to specify a viewer too, in case the game mode is in a 'local player is always blue team' sort of situation)
	 */
	UFUNCTION(BlueprintCallable, Category = "Botani|Teams")
	UBotaniTeamDisplayAsset* GetTeamDisplayAsset(int32 TeamId, int32 ViewerTeamId);

	/**
	 * Gets the team display asset for the specified team, from the perspective of the specified team.
	 * (You have to specify a viewer too, in case the game mode is in a 'local player is always blue team' sort of situation)
	 */
	UFUNCTION(BlueprintCallable, Category = "Botani|Teams")
	UBotaniTeamDisplayAsset* GetEffectiveTeamDisplayAsset(int32 TeamId, UObject* ViewerTeamAgent);

	/**
	 * Tries to change the team for the specified actor.
	 * Only works on authority.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Botani|Teams", meta = (DefaultToSelf = "Actor"))
	bool TryChangeTeamForActor(AActor* Actor, const bool bCreateNew, int32 NewTeamId, int32& OldTeamID) const;

private:
	UPROPERTY()
	TMap<int32, FBotaniTeamTrackingInfo> TeamMap;
};
