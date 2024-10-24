// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widgets/BotaniUserWidget.h"
#include "InventoryTileViewEntry.generated.h"


class UBotaniDraggedWidget;
/**
 * UInventoryTileViewEntry
 *
 * Represents an entry in an inventory tile view.
 */
UCLASS(Abstract)
class UInventoryTileViewEntry : public UBotaniUserWidget, public IUserObjectListEntry
{
	GENERATED_UCLASS_BODY()

public:
	/** Called when the item instance has changed. (Only for the object, not including properties) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory", meta = (DisplayName = "On Item Instance Changed"))
	void OnItemInstanceChanged_BP(class UGameplayInventoryItemInstance* NewInstance);
	
	/** Returns the item instance associated with this tile entry. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UGameplayInventoryItemInstance* GetItemInstance() const;

	/** Returns the item tags of the associated item definition. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FGameplayTagContainer GetItemTags() const;

protected:
	//~ Begin IUserObjectListEntry Interface
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	//~ End IUserObjectListEntry Interface

	//~ Begin UUserWidget Interface
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	//~ End UUserWidget Interface

protected:
	/** The default tags that this entry owns. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	FGameplayTagContainer DefaultTags;
	
	/** The drag-and-drop widget to use for dragging this entry. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Dragging")
	TSoftClassPtr<UBotaniDraggedWidget> DragDropWidgetClass;

	/** The drag-and-drop operation to use for dragging this entry. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Dragging")
	TSubclassOf<class UInventoryDragDropOperation> DragDropOperationClass;
};
