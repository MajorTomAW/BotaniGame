// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Debugging/GameplayDebuggerCategory_Inventory.h"

#include "Instance/GameplayEquipmentInstance.h"
#include "Instance/GameplayInventoryItemInstance.h"

#if WITH_GAMEPLAY_DEBUGGER

#include "Components/GameplayInventoryManager.h"
#include "Components/GameplayEquipmentManager.h"
#include "Library/InventorySystemBlueprintLibrary.h"

FGameplayDebuggerCategory_Inventory::FGameplayDebuggerCategory_Inventory()
{
}

void FGameplayDebuggerCategory_Inventory::CollectData(APlayerController* OwnerPC, AActor* DebugActor)
{
	DataPack.InventoryComponents.Empty();
	
	UGameplayInventoryManager* InventoryManager = UInventorySystemBlueprintLibrary::FindInventoryManager(OwnerPC);
	DataPack.InventoryComponents.Add(InventoryManager);
	
	UGameplayEquipmentManager* EquipmentManager = UInventorySystemBlueprintLibrary::FindEquipmentManager(OwnerPC->GetPawn());
	DataPack.InventoryComponents.Add(EquipmentManager);


	// Collect items
	DataPack.Items.Empty();

	if (!InventoryManager ||!EquipmentManager)
	{
		return;
	}
	
	const TArray<FGameplayInventoryItemSpec>& Items = InventoryManager->GetInventoryListRef();
	for (int32 Idx = 0; Idx < Items.Num(); Idx++)
	{
		const FGameplayInventoryItemSpec& Item = Items[Idx];
		FDebugData::FInventoryItemDebug Debug;

		Debug.Instance = Item.GetInstance() ? Item.GetInstance()->GetName() : TEXT("None");
		if (Debug.Instance.IsEmpty())
		{
			Debug.Instance = TEXT("<Empty Name>");
		}

		if (EquipmentManager->FindEquipmentSpecFromHandle(Item.GetHandle()) == nullptr)
		{
			continue;
		}

		const FGameplayEquipmentSpec& EquipmentSpec = *EquipmentManager->FindEquipmentSpecFromHandle(Item.GetHandle());
		if (EquipmentSpec.IsValid())
		{
			Debug.Equipment = EquipmentSpec.Instance ? EquipmentSpec.Instance->GetName() : TEXT("None");
		}

		if (Debug.Equipment.IsEmpty())
		{
			Debug.Equipment = TEXT("None");
		}
		
		Debug.Item = Item.GetItemDefinition()->DisplayInfo.ItemName.ToString();
		Debug.Quantity = Item.GetStackCount();
		Debug.MaxQuantity = Item.GetMaxStackCount();
		Debug.Handle = Item.GetHandle().ToString();

		DataPack.Items.Add(Debug);
	}
}

void FGameplayDebuggerCategory_Inventory::DrawData(APlayerController* OwnerPC,FGameplayDebuggerCanvasContext& CanvasContext)
{
	FVector2D ViewPortSize;
	GEngine->GameViewport->GetViewportSize(ViewPortSize);

	/*const float BackgroundPadding = 5.0f;
	const FVector2D BackgroundSize(ViewPortSize.X - 2 * BackgroundPadding, ViewPortSize.Y);
	const FLinearColor BackgroundColor(0.1f, 0.1f, 0.1f, 0.8f);

	// Draw a transparent background so that the text is easier to look at
	FCanvasTileItem Background(FVector2D(0.0f, 0.0f), BackgroundSize, BackgroundColor);
	Background.BlendMode = SE_BLEND_Translucent;
	CanvasContext.DrawItem(Background, CanvasContext.DefaultX - BackgroundPadding, CanvasContext.DefaultY - BackgroundPadding);*/
	
	CanvasContext.Printf(TEXT("Inventory Components:"));
	for (const UGameplayInventoryComponent* InventoryComponent : DataPack.InventoryComponents)
	{
		CanvasContext.Printf(TEXT(" - {yellow}%s  {white}Owner: {yellow}%s"), *InventoryComponent->GetName(), *InventoryComponent->GetOwner()->GetName());
	}

	if (DataPack.Items.Num() > 0)
	{
		CanvasContext.Printf(TEXT("Items:"));
		for (int32 Idx = 0; Idx < DataPack.Items.Num(); Idx++)
		{
			const FDebugData::FInventoryItemDebug& ItemData = DataPack.Items[Idx];
			CanvasContext.Printf(TEXT(" - Name: {yellow}%s  {white}Instance: {yellow}%s		{white}Equipment: {yellow}%s		{white}Quantity: {yellow}%d {white}/ {yellow}%d           {white}Handle: {blue}%s"), *ItemData.Item, *ItemData.Instance, *ItemData.Equipment, ItemData.Quantity, ItemData.MaxQuantity, *ItemData.Handle);
		}
	}
}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_Inventory::MakeInstance()
{
	return MakeShareable(new FGameplayDebuggerCategory_Inventory());
}

#endif