// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagStackContainer.h"
#include "UObject/Interface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagStackInterface.generated.h"

struct FGameplayTagStackContainer;

/** Interface for actors that can be tagged with gameplay tag stacks. */
UINTERFACE(BlueprintType, MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UGameplayTagStackInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IGameplayTagStackInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	/**
	 * Get any owned gameplay tag stacks on the actor
	 *
	 * @param TagStackContainer	[OUT] Set of tag stacks on the actor
	 */
	GAMEPLAYTAGSTACKS_API virtual const FGameplayTagStackContainer& GetOwnedGameplayTagStacks() const;

	GAMEPLAYTAGSTACKS_API virtual FGameplayTagStackContainer GetOwnedGameplayTagStacks_Copy() const;

	/**
	 * Check if the actor has at least one stack of the specified tags
	 * @param TagToCheck	Tag to check for a match
	 * @returns True if the actor has a stack of the specified tags, false if not
	 */
	GAMEPLAYTAGSTACKS_API virtual bool HasMatchingGameplayTagStack(FGameplayTag TagToCheck) const;

	/**
	 * Returns the stack count of the specified tag
	 * Or 0 if the tag is not found
	 * @param TagToCheck Tag to check for a match
	 * @returns The stack count of the specified tag (0 if not found)
	 */
	GAMEPLAYTAGSTACKS_API virtual int32 GetGameplayTagStackCount(FGameplayTag TagToCheck) const;

	/**
	 * Adds a specified number of stacks of the specified tag to the actor.
	 * Does nothing if StackCount is below 1
	 * @param Tag Tag to add stacks to
	 * @param StackCount Number of stacks to add
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = GameplayTags)
	GAMEPLAYTAGSTACKS_API virtual void AddGameplayTagStack(FGameplayTag Tag, int32 StackCount);

	/**
	 * Sets the stack count of the specified tag to the specified value.
	 * @param Tag Tag to set the stack count of
	 * @param StackCount New stack count
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = GameplayTags)
	GAMEPLAYTAGSTACKS_API virtual void SetGameplayTagStack(FGameplayTag Tag, int32 StackCount);

	/**
	 * Removes a specified number of stacks of the specified tag from the actor.
	 * Does nothing if StackCount is below 1
	 * @param Tag Tag to remove stacks from
	 * @param StackCount Number of stacks to remove
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = GameplayTags)
	GAMEPLAYTAGSTACKS_API virtual void RemoveGameplayTagStack(FGameplayTag Tag, int32 StackCount);
};

/** Blueprint function library for working with gameplay tag stacks (not everything is exposed to bp tho). */
UCLASS(MinimalAPI)
class UGameplayTagStackLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Get any owned gameplay tag stacks on the actor
	 * @param Target	Actor to get tag stacks from
	 * @param TagStackContainer	[OUT] Set of tag stacks on the actor
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = GameplayTags, meta = (DefaultToSelf = "Target"))
	static GAMEPLAYTAGSTACKS_API void GetOwnedGameplayTagStacks(UObject* Target, FGameplayTagStackContainer& TagStackContainer);

	/**
	 * Check if the actor has at least one stack of the specified tags
	 * @param Target	Actor to get tag stacks from
	 * @param TagToCheck	Tag to check for a match
	 * @returns True if the actor has a stack of the specified tags, false if not
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = GameplayTags, meta = (DefaultToSelf = "Target"))
	static GAMEPLAYTAGSTACKS_API bool HasMatchingGameplayTagStack(UObject* Target, const FGameplayTag TagToCheck);

	/**
	 * Check if that tag stack container has at least one stack of the specified tags
	 * @param TagStackContainer	Tag stack container to check for a match
	 * @param TagToCheck	Tag to check for a match
	 * @returns True if the tag stack container has a stack of the specified tags, false if not
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = GameplayTags)
	static GAMEPLAYTAGSTACKS_API bool HasMatchingGameplayTagStackContainer(const FGameplayTagStackContainer& TagStackContainer, const FGameplayTag TagToCheck);

	/**
	 * Returns the stack count of the specified tag.
	 * Or 0 if the tag is not found.
	 * @param Target	Actor to get tag stacks from
	 * @param TagToCheck Tag to check for a match
	 * @returns The stack count of the specified tag (0 if not found)
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = GameplayTags, meta = (DefaultToSelf = "Target"))
	static GAMEPLAYTAGSTACKS_API int32 GetGameplayTagStackCount(UObject* Target, const FGameplayTag TagToCheck);

	/**
	 * Returns the stack count of the specified tag.
	 * Or 0 if the tag is not found.
	 * @param TagStackContainer	Tag stack container to check for a match
	 * @param TagToCheck Tag to check for a match
	 * @returns The stack count of the specified tag (0 if not found)
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = GameplayTags)
	static GAMEPLAYTAGSTACKS_API int32 GetGameplayTagStackCountContainer(const FGameplayTagStackContainer& TagStackContainer, const FGameplayTag TagToCheck);

	/**
	 * Returns the list of tag stacks in the given tag stack container.
	 * @param TagStackContainer Tag stack container to get the stacks from
	 * @returns The list of tag stacks in the given tag stack container
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = GameplayTags)
	static GAMEPLAYTAGSTACKS_API TArray<FGameplayTagStack> GetGameplayTagStacks(const FGameplayTagStackContainer& TagStackContainer);

	/**
	 * Breaks a tag stack into its tag and stack count.
	 * @param TagStack Tag stack to break
	 * @param [OUT] Tag The tag of the tag stack
	 * @param [OUT] StackCount The stack count of the tag stack
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = GameplayTags)
	static GAMEPLAYTAGSTACKS_API void BreakGameplayTagStack(const FGameplayTagStack& TagStack, FGameplayTag& Tag, int32& StackCount);
};