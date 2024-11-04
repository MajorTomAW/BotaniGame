// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "BotaniWeaponModManager.generated.h"

class UBotaniWeaponModInstance;

/**
 * FBotaniWeaponModSpec
 *
 * Represents a single, active weapon modification instance in the inventory.
 */
USTRUCT()
struct FBotaniWeaponModSpec : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FBotaniWeaponModSpec()
	{
	}

private:
	friend struct FBotaniWeaponModList;

	/** The active weapon modification instance. */
	UPROPERTY()
	TObjectPtr<UBotaniWeaponModInstance> ModInstance = nullptr;

	/** The stack count of this weapon modification. */
	UPROPERTY()
	int32 StackCount = 0;
};

/**
 * FBotaniWeaponModList
 *
 * Represents a list of active weapon modifications in the inventory.
 */
USTRUCT(BlueprintType)
struct FBotaniWeaponModList : public FFastArraySerializer
{
	GENERATED_BODY()
	friend class UBotaniWeaponModManager;

	FBotaniWeaponModList()
	{
	}

	FBotaniWeaponModList(UBotaniWeaponModManager* InManager)
		: ModManager(InManager)
	{
	}

public:
	//~ Begin FFastArraySerializer Interface
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FBotaniWeaponModSpec, FBotaniWeaponModList>(Modifications, DeltaParams, *this);
	}
	//~ End FFastArraySerializer Interface

private:
	/** The list of active weapon modifications. */
	UPROPERTY()
	TArray<FBotaniWeaponModSpec> Modifications;
	
	/** The weapon modification manager that owns this list. */
	UPROPERTY(NotReplicated)
	TObjectPtr<UBotaniWeaponModManager> ModManager = nullptr;
};

template<>
struct TStructOpsTypeTraits<FBotaniWeaponModList> : public TStructOpsTypeTraitsBase2<FBotaniWeaponModList>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

/**
 * UBotaniWeaponModManager
 *
 * Manages weapon modifications on the owning actor.
 */
UCLASS(ClassGroup = ("Botani"), meta = (BlueprintSpawnableComponent))
class BOTANIGAME_API UBotaniWeaponModManager : public UPawnComponent
{
	GENERATED_UCLASS_BODY()

public:
	static UBotaniWeaponModManager* FindWeaponModManager(const AActor* Actor);
	
	//~ Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface
	
protected:
private:
	/** The list of active weapon modifications. */
	UPROPERTY(Replicated)
	FBotaniWeaponModList WeaponModifications;
};
