// Copyright © 2024 MajorT. All rights reserved.

#pragma once
#include "GameFramework/PlayerState.h"
#include "Instance/GameplayInventoryItemInstance.h"

#include "GameplayInventoryMessages.generated.h"

class UGameplayInventoryItemInstance;

/**
 * FGameplayInventoryChangeMessage
 *
 * A message that is sent when an item is added, removed, or changed on the inventory.
 */
USTRUCT(BlueprintType)
struct FGameplayInventoryChangeMessage
{
	GENERATED_BODY()

	FGameplayInventoryChangeMessage() = default;
	FGameplayInventoryChangeMessage(UGameplayInventoryItemInstance* InInstance, const int32 InOldCount, const int32 InNewCount);

public:
	/** The actor that the inventory change occurred on */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<AActor> InventoryOwner = nullptr;

	/** The controller that the inventory change occurred on */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<AController> InventoryController = nullptr;

	/** The inventory manager component that the inventory change occurred on */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<class UGameplayInventoryComponent> InventoryManager = nullptr;

	/** The item instance that was added, removed, or changed */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UGameplayInventoryItemInstance> ItemInstance = nullptr;

	/** The new stack count of the item instance */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 NewStackCount = INDEX_NONE;

	/** The old stack count of the item instance */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 OldStackCount = INDEX_NONE;

	/** The delta in stack count */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 Delta = 0;

public:
	bool operator==(const FGameplayInventoryChangeMessage& Other) const
	{
		return (ItemInstance == Other.ItemInstance) &&
			(NewStackCount == Other.NewStackCount) &&
				(OldStackCount == Other.OldStackCount) &&
					(Delta == Other.Delta);
	}

	static FGameplayInventoryChangeMessage Make(UGameplayInventoryItemInstance* InInstance, const int32 InOldCount, const int32 InNewCount);
};

inline FGameplayInventoryChangeMessage::FGameplayInventoryChangeMessage(UGameplayInventoryItemInstance* InInstance, const int32 InOldCount, const int32 InNewCount)
{
	NewStackCount = InNewCount;
	OldStackCount = InOldCount;
	Delta = InNewCount - InOldCount;
	ItemInstance = InInstance;
	InventoryManager = InInstance->GetOwnerComponent<UGameplayInventoryComponent>();
	InventoryOwner = InventoryManager->GetOwner();

	if (AController* Controller = Cast<AController>(InventoryOwner))
	{
		InventoryController = Controller;
	}
	else if (const APawn* Pawn = Cast<APawn>(InventoryOwner))
	{
		InventoryController = Pawn->GetController();
	}
	else if (APlayerState* PlayerState = Cast<APlayerState>(InventoryOwner))
	{
		InventoryController = PlayerState->GetPawn()->GetController();
	}
}

inline FGameplayInventoryChangeMessage FGameplayInventoryChangeMessage::Make(UGameplayInventoryItemInstance* InInstance, const int32 InOldCount, const int32 InNewCount)
{
	FGameplayInventoryChangeMessage Message;
	Message.NewStackCount = InNewCount;
	Message.OldStackCount = InOldCount;
	Message.Delta = InNewCount - InOldCount;
	Message.ItemInstance = InInstance;
	Message.InventoryManager = InInstance->GetOwnerComponent<UGameplayInventoryComponent>();
	Message.InventoryOwner = Message.InventoryManager->GetOwner();
	
	if (AController* Controller = Cast<AController>(Message.InventoryOwner))
	{
		Message.InventoryController = Controller;
	}
	else if (const APawn* Pawn = Cast<APawn>(Message.InventoryOwner))
	{
		Message.InventoryController = Pawn->GetController();
	}
	else if (APlayerState* PlayerState = Cast<APlayerState>(Message.InventoryOwner))
	{
		Message.InventoryController = PlayerState->GetPawn()->GetController();
	}

	return Message;
}
