// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once
#include "GameplayDebuggerCategory.h"


class UGameplayInventoryComponent;

class FGameplayDebuggerCategory_Inventory : public FGameplayDebuggerCategory
{
public:
	FGameplayDebuggerCategory_Inventory();

	virtual void CollectData(APlayerController* OwnerPC, AActor* DebugActor) override;
	virtual void DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) override;

	static TSharedRef<FGameplayDebuggerCategory> MakeInstance();

protected:
	struct FDebugData
	{
		TArray<UGameplayInventoryComponent*> InventoryComponents;

		struct FInventoryItemDebug
		{
			FString Item;
			FString Instance;
			FString Equipment;
			int32 Quantity;
			int32 MaxQuantity;
			FString Handle;
		};

		TArray<FInventoryItemDebug> Items;
	};

	FDebugData DataPack;
};
