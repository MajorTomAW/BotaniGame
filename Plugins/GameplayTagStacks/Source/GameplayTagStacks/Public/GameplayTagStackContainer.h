// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "GameplayTagContainer.h" 
#include "Net/Serialization/FastArraySerializer.h"

#include "GameplayTagStackContainer.generated.h"

struct FGameplayTagStackContainer;
struct FNetDeltaSerializeInfo;

/**
 * Represents a change in a gameplay tag stack
 */
USTRUCT(BlueprintType)
struct FGameplayTagStackChangeMessage
{
	GENERATED_BODY()

	FGameplayTagStackChangeMessage()
		: Owner(nullptr)
		, Tag(FGameplayTag::EmptyTag)
		, NewCount(0)
		, OldCount(0)
		, Delta(0)
	{
	}

	FGameplayTagStackChangeMessage(UObject* InOwner, const FGameplayTag& Tag, const int32& NewCount, const int32 OldCount)
		: Owner(InOwner)
		, Tag(Tag)
		, NewCount(NewCount)
		, OldCount(OldCount)
		, Delta(NewCount - OldCount)
	{
	}

	/** The owning actor of the tag stack container */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GameplayTagStacks")
	TObjectPtr<UObject> Owner;

	/** The tag that was changed */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GameplayTagStacks")
	FGameplayTag Tag;

	/** The new stack count */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GameplayTagStacks")
	int32 NewCount;

	/** The old stack count */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GameplayTagStacks")
	int32 OldCount;

	/** The delta in the stack count */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GameplayTagStacks")
	int32 Delta;
};

/**
 * Represents one stack of a gameplay tag (tag + count)
 */
USTRUCT(BlueprintType)
struct GAMEPLAYTAGSTACKS_API FGameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FGameplayTagStack()
	{
	}

	FGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
		: Tag(InTag)
		, StackCount(InStackCount)
	{
	}

	/** Returns the tag of the stack */
	const FGameplayTag& GetTag() const
	{
		return Tag;
	}

	/** Returns the stack count */
	int32 GetStackCount() const
	{
		return StackCount;
	}

	bool operator==(const FGameplayTagStack& Other) const
	{
		return Tag.MatchesTagExact(Other.Tag) && StackCount == Other.StackCount;
	}
	
	FString GetDebugString() const;

private:
	friend FGameplayTagStackContainer;

	UPROPERTY()
	FGameplayTag Tag = FGameplayTag::EmptyTag;

	UPROPERTY()
	int32 StackCount = 0;
};

/**
 * Container of gameplay tag stacks
 */
USTRUCT(BlueprintType)
struct GAMEPLAYTAGSTACKS_API FGameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY()

	FGameplayTagStackContainer(UObject* InOwner = nullptr)
		: Owner(InOwner)
	{
	}

public:
	/** Adds a specified number of stacks to the tag (does nothing if StackCount is below 1) */
	void AddStack(FGameplayTag Tag, int32 StackCount);

	/** Removes a specified number of stacks from the tag (does nothing if StackCount is below 1) */
	void RemoveStack(FGameplayTag Tag, int32 StackCount);

	/** Sets the stack count of the specified tag to the specified value */
	void SetStack(FGameplayTag Tag, int32 StackCount);

	/** Returns the list of tag stacks */
	const TArray<FGameplayTagStack>& GetStacks() const
	{
		return Stacks;
	}

	/** Returns the stack count of the specified tag (or 0 if the tag is not present) */
	int32 GetStackCount(FGameplayTag Tag) const
	{
		return TagToCountMap.FindRef(Tag);
	}

	/** Returns true if there is at least one stack of the specified tag */
	bool ContainsTag(FGameplayTag Tag) const
	{
		return TagToCountMap.Contains(Tag);
	}
	
	/** Resets the tag stack container */ 
	void Reset()
	{
		Stacks.Reset();
		TagToCountMap.Reset();
	}

	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FGameplayTagStack, FGameplayTagStackContainer>(Stacks, DeltaParams, *this);
	}

private:
	void BroadcastChangeMessage(const FGameplayTag& Tag, int32 NewCount, int32 OldCount) const;

private:
	/** Replicated list of gameplay tag stacks */
	UPROPERTY()
	TArray<FGameplayTagStack> Stacks;
	
	/** Faster list of tag stacks for queries */
	TMap<FGameplayTag, int32> TagToCountMap;

	/** The owner of this container */
	UPROPERTY(NotReplicated)
	TObjectPtr<UObject> Owner;
};

template<>
struct TStructOpsTypeTraits<FGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FGameplayTagStackContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};