// Copyright © 2024 Botanibots Team. All rights reserved.


#include "UI/DragDrop/InventoryDragDropOperation.h"

#include "Inventory/Instances/BotaniItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryDragDropOperation)

UInventoryDragDropOperation::UInventoryDragDropOperation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Pivot = EDragPivot::CenterCenter;
}