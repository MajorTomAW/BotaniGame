// Copyright © 2024 MajorT. All rights reserved.

#pragma once
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayInventoryItemSpec.h"
#include "GameplayInventoryItemSpecHandle.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "GameplayEquipmentSpec.generated.h"

class UGameplayEquipmentDefinition;

/**
 * FGameplayEquipmentGrantedHandles
 *
 * Helper struct to cache all granted ability handles, effects, and attributes that were given by the equipment
 */
USTRUCT(BlueprintType)
struct FGameplayEquipmentGrantedHandles
{
	GENERATED_BODY()
	// Giving and taking away these handles should be managed by an ability set

public:
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilityHandles;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> EffectHandles;

	UPROPERTY()
	TArray<TObjectPtr<const class UAttributeSet>> AttributeSets;
};

/**
 * FGameplayEquipmentSpec
 *
 * A spec for an equipment item.
 */
USTRUCT(BlueprintType)
struct GAMEPLAYINVENTORYSYSTEM_API FGameplayEquipmentSpec : public FFastArraySerializerItem
{
	GENERATED_BODY()
	friend class UGameplayEquipmentInstance;
	friend class UGameplayEquipmentManager;
	friend struct FGameplayEquipmentSpecContainer;

	FGameplayEquipmentSpec()
	{
	}

	FGameplayEquipmentSpec(const FGameplayInventoryItemSpec& Spec);

	/** Returns a debug string */
	FString GetDebugString() const;

	void SetGrantedHandles(const FGameplayEquipmentGrantedHandles& InGrantedHandles)
	{
		GrantedHandles = InGrantedHandles;
	}

	//~ Begin FFastArraySerializerItem Interface
	void PostReplicatedAdd(const struct FGameplayEquipmentSpecContainer& InArraySerializer);
	void PostReplicatedRemove(const struct FGameplayEquipmentSpecContainer& InArraySerializer);
	void PostReplicatedChange(const struct FGameplayEquipmentSpecContainer& InArraySerializer);
	//~ End FFastArraySerializerItem Interface

public:
	/** Returns the equipment instance */
	UGameplayEquipmentInstance* GetInstance() const { return Instance; }

public:
	/** The item spec that this equipment is for */
	UPROPERTY()
	FGameplayInventoryItemSpec OwningItemSpec;
	
	/** The equipment class */
	UPROPERTY()
	TObjectPtr<UGameplayEquipmentDefinition> EquipmentDefinition;

	/** The equipment instance */
	UPROPERTY()
	TObjectPtr<UGameplayEquipmentInstance> Instance = nullptr;

	/** The item spec handle that we belong to. */
	UPROPERTY()
	FGameplayInventoryItemSpecHandle Handle;

	/** Authority-only list of granted handles */
	UPROPERTY(NotReplicated)
	FGameplayEquipmentGrantedHandles GrantedHandles;

public:
	bool IsValid() const
	{
		return (Instance != nullptr) &&
			(Handle.IsValid());
	}
	
	bool operator==(const FGameplayEquipmentSpec& Other) const
	{
		return (Instance == Other.Instance) &&
			(Handle == Other.Handle);
	}
};

/**
 * FGameplayEquipmentSpecContainer
 *
 * A list of equipment entries
 */
USTRUCT(BlueprintType)
struct FGameplayEquipmentSpecContainer : public FFastArraySerializer
{
	GENERATED_BODY()
	friend class UGameplayEquipmentManager;

	FGameplayEquipmentSpecContainer();

	explicit FGameplayEquipmentSpecContainer(UGameplayEquipmentManager* InOwner)
		: Owner(InOwner)
	{
	}

public:
	//~ Begin FFastArraySerializer Interface
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~ End FFastArraySerializer Interface
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGameplayEquipmentSpec, FGameplayEquipmentSpecContainer>(Items, DeltaParams, *this);
	}

	const FGameplayEquipmentSpec* FindSpecByHandle(const FGameplayInventoryItemSpecHandle& Handle) const
	{
		for (const FGameplayEquipmentSpec& Spec : Items)
		{
			if (Spec.Handle == Handle)
			{
				return &Spec;
			}
		}

		return nullptr;
	}


public:
	/** The list of equipment items. */
	UPROPERTY()
	TArray<FGameplayEquipmentSpec> Items;

	/** The owner component */
	UPROPERTY(NotReplicated)
	TObjectPtr<UGameplayEquipmentManager> Owner;
};

template<>
struct TStructOpsTypeTraits<FGameplayEquipmentSpecContainer> : public TStructOpsTypeTraitsBase2<FGameplayEquipmentSpecContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};