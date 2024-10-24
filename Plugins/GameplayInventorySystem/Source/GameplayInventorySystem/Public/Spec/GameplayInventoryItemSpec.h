// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "GameplayInventoryItemSpecHandle.h"
#include "Net/Serialization/FastArraySerializer.h"


#include "GameplayInventoryItemSpec.generated.h"

class UGameplayInventoryItemDefinition;

/**
 * FGameplayInventoryItemSpec
 *
 * Represents a single item instance in the inventory.
 */
USTRUCT(BlueprintType)
struct GAMEPLAYINVENTORYSYSTEM_API FGameplayInventoryItemSpec : public FFastArraySerializerItem
{
	GENERATED_BODY()
	friend struct FGameplayInventoryItemContainer;
	friend class UGameplayInventoryItemInstance;
	friend class UGameplayInventoryManager;

	FGameplayInventoryItemSpec();

	FGameplayInventoryItemSpec(UGameplayInventoryItemDefinition* InItemDef, int32 InStackCount, UObject* InSourceObject);

public:
	bool IsValid() const
	{
		return Instance != nullptr;
	}

	bool operator==(const FGameplayInventoryItemSpec& Other) const
	{
		return Instance == Other.Instance;
	}

	//~ Begin FFastArraySerializerItem Interface
	void PostReplicatedAdd(const struct FGameplayInventoryItemContainer& InArraySerializer);
	void PostReplicatedRemove(const struct FGameplayInventoryItemContainer& InArraySerializer);
	void PostReplicatedChange(const struct FGameplayInventoryItemContainer& InArraySerializer);
	//~ End FFastArraySerializerItem Interface

	/** Returns whether this item stack is full */
	bool IsStackFull() const;

	/** Returns the current stack count of this item */
	int32 GetStackCount() const { return FMath::Max(1, StackCount); }

	/** Returns the max stack count of this item */
	int32 GetMaxStackCount() const;

	/** Returns the handle for this item */
	FGameplayInventoryItemSpecHandle GetItemHandle() const { return Handle; }

	/** Returns the item instance */
	UGameplayInventoryItemInstance* GetInstance() const { return Instance; }
	UGameplayInventoryItemDefinition* GetItemDefinition() const { return Item; }
	FGameplayInventoryItemSpecHandle GetHandle() const { return Handle; }

private:
	/** Replicated item instance. */
	UPROPERTY()
	TObjectPtr<class UGameplayInventoryItemInstance> Instance;

	/** Replicated item definition. */
	UPROPERTY()
	TObjectPtr<class UGameplayInventoryItemDefinition> Item;

	/** Object this item was created from. Can be an actor or static object. */
	UPROPERTY()
	TWeakObjectPtr<UObject> SourceObject;

	/** Stack count of this instance. */
	UPROPERTY()
	int32 StackCount = 0;

	/** The last observed stack count. */
	UPROPERTY(NotReplicated)
	int32 LastObservedStackCount = INDEX_NONE;

	/** The handle for outside references to this item. */
	UPROPERTY()
	FGameplayInventoryItemSpecHandle Handle;
};

/**
 * FGameplayInventoryItemSpecContainer
 *
 * Represents a list of item instances in the inventory.
 */
USTRUCT(BlueprintType)
struct FGameplayInventoryItemContainer : public FFastArraySerializer
{
	GENERATED_BODY()
	friend class UGameplayInventoryComponent;
	friend class UGameplayInventoryManager;
	friend FGameplayInventoryItemSpec;
	
	FGameplayInventoryItemContainer();
	FGameplayInventoryItemContainer(UGameplayInventoryManager* InOwnerComponent);


public:
	//~ Begin FFastArraySerializer Interface
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~ End FFastArraySerializer Interface

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGameplayInventoryItemSpec, FGameplayInventoryItemContainer>(Items, DeltaParams, *this);
	}

public:
	TArray<UGameplayInventoryItemInstance*> GetAllInstances() const;

private:
	/** Replicated list of items */
	UPROPERTY()
	TArray<FGameplayInventoryItemSpec> Items;

	/** The owner component of this inventory */
	UPROPERTY(NotReplicated)
	TObjectPtr<UGameplayInventoryManager> OwnerComponent;

private:
	/** Constructs and broadcasts a change message for the given entry */
	void BroadcastInventoryChangeMessage(UGameplayInventoryItemInstance* InItemInstance, int32 InOldCount, int32 InNewCount) const;
};

template<>
struct TStructOpsTypeTraits<FGameplayInventoryItemContainer> : public TStructOpsTypeTraitsBase2<FGameplayInventoryItemContainer>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};