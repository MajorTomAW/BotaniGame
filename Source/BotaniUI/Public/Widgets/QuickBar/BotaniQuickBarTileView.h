// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniQuickBarTileViewEntry.h"
#include "CommonTileView.h"
#include "Spec/GameplayInventoryItemSpec.h"
#include "Spec/GameplayInventoryItemSpecHandle.h"
#include "BotaniQuickBarTileView.generated.h"

class UBotaniQuickBarComponent;
class UBotaniInventoryManager;
struct FGameplayInventoryItemSpec;

/**
 * UBotaniQuickBarEntryData
 *
 * Data object for a quick bar entry.
 */
UCLASS(NotBlueprintable, BlueprintType, MinimalAPI)
class UBotaniQuickBarEntryData : public UObject
{
	GENERATED_BODY()
	friend class UBotaniQuickBarTileView;

public:
	UBotaniQuickBarEntryData();

	void Initialize(const FGameplayInventoryItemSpec& InItemSpec);

public:
	UPROPERTY(BlueprintReadOnly, Category = "Quick Bar")
	FGameplayInventoryItemSpecHandle Handle;

	UPROPERTY(BlueprintReadOnly, Category = "Quick Bar")
	int32 SlotID = -1;

protected:
	FGameplayInventoryItemSpec ItemSpec;
};

/**
 * UBotaniQuickBarTileView
 *
 * A tile view that displays entries for the quick bar.
 */
UCLASS(MinimalAPI)
class UBotaniQuickBarTileView : public UCommonTileView
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Quick Bar", meta = (ClampMin = "-1", UIMin = "-1", UIMax = "1", ClampMax = "1"))
	int32 TileDirection = 1;

public:
	/** Initializes the tile view with the given inventory manager. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Quick Bar")
	void InitializeWithInventoryManager(UBotaniInventoryManager* InInventoryManager, UBotaniQuickBarComponent* InQuickBar);
	
	/** Returns the entry widget for the given item spec handle. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Quick Bar")
	UBotaniQuickBarTileViewEntry* FindEntryWidgetByHandle(const FGameplayInventoryItemSpecHandle& Handle) const;

	/** Returns the entry data object for the given item spec handle. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Quick Bar")
	UBotaniQuickBarEntryData* FindEntryDataByHandle(const FGameplayInventoryItemSpecHandle& Handle) const;

protected:
	virtual void AddInitialEntries();

private:
	UPROPERTY()
	TObjectPtr<UBotaniInventoryManager> InventoryManager;

	UPROPERTY()
	TObjectPtr<UBotaniQuickBarComponent> QuickBar;
};
