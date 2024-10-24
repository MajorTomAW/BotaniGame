// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "GameplayInventoryItemSpecHandle.generated.h"

/**
 * Globally unique handle that points to a specific item spec in the inventory.
 *
 * With the usage of this handle, we can refer to a specific item spec in the inventory in a very efficient way,
 * without having to store the item spec itself.
 * This is useful in networking scenarios where we want to refer to an item spec in the inventory without having to replicate the item spec itself.
 */

USTRUCT(BlueprintType)
struct FGameplayInventoryItemSpecHandle
{
	GENERATED_USTRUCT_BODY()

	/** Sets this to a valid handle */
	void GenerateNewHandle();
	
	FGameplayInventoryItemSpecHandle()
		: Handle(INDEX_NONE)
	{
	}
	
	/** True if GenerateNewHandle was called on this handle */
	bool IsValid() const
    {
    	return Handle != INDEX_NONE;
    }

	void Invalidate()
	{
		Handle = INDEX_NONE;
	}

public:

	/** Operator to expose FInventoryItemSpecHandle serialization to custom serialization functions like NetSerialize overrides. */
	friend FArchive& operator<<(FArchive& Ar, FGameplayInventoryItemSpecHandle& Value)
	{
		static_assert(sizeof(FGameplayInventoryItemSpecHandle) == 4, "If properties of FInventoryItemSpecHandle change, consider updating this operator implementation.");
		Ar << Value.Handle;
		return Ar;
	}

	friend uint32 GetTypeHash(const FGameplayInventoryItemSpecHandle& SpecHandle)
	{
		return ::GetTypeHash(SpecHandle.Handle);
	}
	
	bool operator==(const FGameplayInventoryItemSpecHandle& Other) const
    {
    	return Handle == Other.Handle;
    }
	
	FString ToString() const
    {
    	return IsValid() ? FString::FromInt(Handle) : TEXT("Invalid");
    }

private:
	/** Handle to the item spec. */
	UPROPERTY()
	int32 Handle;
};
