// Copyright © 2024 Botanibots Team. All rights reserved.


#include "UI/InventoryTileViewEntry.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UI/InventoryTileView.h"
#include "UI/DragDrop/InventoryDragDropOperation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryTileViewEntry)

UInventoryTileViewEntry::UInventoryTileViewEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UGameplayInventoryItemInstance* UInventoryTileViewEntry::GetItemInstance() const
{
	if (const UInventoryTileEntryObject* Entry = GetListItem<UInventoryTileEntryObject>())
	{
		return Entry->GetItemInstance();
	}

	return nullptr;
}

FGameplayTagContainer UInventoryTileViewEntry::GetItemTags() const
{
	FGameplayTagContainer Tags = DefaultTags;

	if (UGameplayInventoryItemInstance* Instance = GetItemInstance())
	{
		Tags.AppendTags(Instance->GetItemDefinition()->GetGameplayTags());
	}
	
	return Tags;
}

void UInventoryTileViewEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	OnItemInstanceChanged_BP(GetListItem<UInventoryTileEntryObject>()->GetItemInstance());
}

FReply UInventoryTileViewEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = FReply::Unhandled();

	// Only LMB is allowed to start dragging
	if ((InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton) &&
		!InMouseEvent.IsTouchEvent())
	{
		return Reply;
	}

	// If we have no item instance, means this slot is empty.
	// So why bother dragging a 'null' item?
	if (GetItemInstance() == nullptr)
	{
		return Reply;
	}

	// Detect dragging
	TSharedPtr<SWidget> SlateDragDrop = GetCachedWidget();
	if (SlateDragDrop.IsValid())
	{
		FEventReply EventReply;
		EventReply.NativeReply = FReply::Handled();
		EventReply.NativeReply.DetectDrag(SlateDragDrop.ToSharedRef(), EKeys::LeftMouseButton);
	}

	return Reply;
}

void UInventoryTileViewEntry::NativeOnDragDetected(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	// Make sure we have a valid drag drop widget class.
	if (DragDropWidgetClass.IsNull() ||
		DragDropOperationClass == nullptr)
	{
		ensureMsgf(false, TEXT("[%hs] DragDropWidgetClass is not set!"), __FUNCTION__);
		return;
	}
	
	// Again, make sure we have an item to drag.
	UGameplayInventoryItemInstance* ItemInstance = GetItemInstance();
	if (ItemInstance == nullptr)
	{
		return;
	}

	UClass* DragDropWidgetClassObj = DragDropWidgetClass.LoadSynchronous();
	UBotaniDraggedWidget* DragDropWidget = CreateWidget<UBotaniDraggedWidget>(GetOwningPlayer(), DragDropWidgetClassObj);

	UInventoryDragDropOperation* InventoryDropOp = NewObject<UInventoryDragDropOperation>(GetTransientPackage(), DragDropOperationClass);
	{
		InventoryDropOp->Tag = TEXT("InventoryDragDropOperation");
		InventoryDropOp->Payload = ItemInstance;
		InventoryDropOp->DefaultDragVisual = DragDropWidget;
		InventoryDropOp->Pivot = EDragPivot::MouseDown;
		InventoryDropOp->Offset = FVector2D::ZeroVector;

		InventoryDropOp->HostingWidget = this;
	}

	DragDropWidget->OnDraggedWidgetCreated(InventoryDropOp);

	OutOperation = InventoryDropOp;
}

void UInventoryTileViewEntry::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
}

void UInventoryTileViewEntry::NativeOnDragEnter(
	const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
}

void UInventoryTileViewEntry::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
}

bool UInventoryTileViewEntry::NativeOnDrop(
	const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

bool UInventoryTileViewEntry::NativeOnDragOver(
	const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
}
