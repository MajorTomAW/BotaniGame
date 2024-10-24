// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "Spec/GameplayInventoryItemSpecHandle.h"
#include "BotaniQuickBarComponent.generated.h"

class UBotaniInventoryManager;
class UBotaniEquipmentManager;
class UBotaniEquipmentInstance;
class UBotaniItemInstance;

/**
 * UBotaniQuickBarComponent
 *
 * An inventory component that represents the quick bar of the player.
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class BOTANIGAME_API UBotaniQuickBarComponent : public UControllerComponent
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	//~ End UActorComponent Interface

	/** Called to cycle the active slot forward */
	UFUNCTION(BlueprintCallable, Category = "Botani|Quick Bar")
	void CycleActiveSlotForward();

	/** Called to cycle the active slot backward */
	UFUNCTION(BlueprintCallable, Category = "Botani|Quick Bar")
	void CycleActiveSlotBackward();

	/** Called to set the active slot index */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Botani|Quick Bar")
	void SetActiveSlotIndex(int32 NewIndex);

	/** Returns the slots in the quick bar */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Quick Bar")
	TArray<UBotaniItemInstance*> GetSlots() const { return Slots; }

	/** Returns the active slot index */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Quick Bar")
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	/** Returns the item handle in the active slot */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Quick Bar")
	FGameplayInventoryItemSpecHandle GetActiveSlotItemHandle() const { return EquippedItemHandle; }

	/** Returns the number of slots in the quick bar */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Quick Bar")
	int32 GetNumSlots() const { return NumSlots; }

	/** Returns the next free slot index */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Quick Bar")
	int32 GetNextFreeItemSlot() const;

	/** Adds an item by Handle to the specified slot */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Botani|Quick Bar")
	void AddItemToSlot(const int32 SlotIndex, const FGameplayInventoryItemSpecHandle& ItemHandle);

	/** Removes the item from the specified slot */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Botani|Quick Bar")
	UBotaniItemInstance* RemoveItemFromSlot(const int32 SlotIndex);

	/** Removes the item via Handle */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Botani|Quick Bar")
	UBotaniItemInstance* RemoveItemByHandle(const FGameplayInventoryItemSpecHandle& ItemHandle);
	
	
protected:
	/** The number of slots in the quick bar */
	UPROPERTY(EditDefaultsOnly, Category = "Quick Bar", meta = (ClampMin = 0, UIMin = 1))
	int32 NumSlots = 3;

	UFUNCTION()
	void OnRep_Slots();

	UFUNCTION()
	void OnRep_ActiveSlotIndex();

private:
	void UnequipItemInSlot();
	void EquipItemInSlot();

	UBotaniInventoryManager* FindInventoryManager() const;
	UBotaniEquipmentManager* FindEquipmentManager() const;
	AController* GetOwnerController() const;

	void BroadcastQuickBarMessage(const struct FGameplayTag& MessageTag);
	
private:
	/** Replicated list of slots in the quick bar */
	UPROPERTY(ReplicatedUsing = OnRep_Slots)
	TArray<TObjectPtr<UBotaniItemInstance>> Slots;

	/** The index of the active slot */
	UPROPERTY(ReplicatedUsing = OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex = -1;

	UPROPERTY()
	FGameplayInventoryItemSpecHandle EquippedItemHandle;
};

/**
 * FBotaniQuickBarChangeMessage
 *
 * A generic message that is sent when the quick bar or its slots change.
 */
USTRUCT(BlueprintType)
struct FBotaniQuickBarChangeMessage
{
	GENERATED_BODY()

public:
	/** The owner of the quick bar */
	UPROPERTY(BlueprintReadOnly, Category = "Botani|Quick Bar")
	TObjectPtr<AActor> Owner = nullptr;

	/** The QuickBar component */
	UPROPERTY(BlueprintReadOnly, Category = "Botani|Quick Bar")
	UBotaniQuickBarComponent* QuickBar = nullptr;

	/** The slots in the quick bar */
	UPROPERTY(BlueprintReadOnly, Category = "Botani|Quick Bar")
	TArray<TObjectPtr<UBotaniItemInstance>> Slots;

	/** The active slot index */
	UPROPERTY(BlueprintReadOnly, Category = "Botani|Quick Bar")
	int32 ActiveSlotIndex = -1;

	/** The item handle in the active slot */
	UPROPERTY(BlueprintReadOnly, Category = "Botani|Quick Bar")
	FGameplayInventoryItemSpecHandle EquippedItemHandle;
};
