// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widgets/BotaniTaggedWidget.h"
#include "BotaniQuickBarTileViewEntry.generated.h"

class UBotaniQuickBarComponent;
class UBotaniQuickBarEntryData;
/**
 * UBotaniQuickBarTileViewEntry
 *
 * A single entry in the quick bar tile view.
 */
UCLASS(Abstract, MinimalAPI)
class UBotaniQuickBarTileViewEntry : public UBotaniTaggedWidget, public IUserObjectListEntry
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget Interface
	
	//~ Begin IUserObjectListEntry Interface
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	//~ End IUserObjectListEntry Interface

protected:
	/** Called when the entry data is set */
	UFUNCTION(BlueprintImplementableEvent, Category = "Quick Bar")
	void OnEntryDataSet(const UBotaniQuickBarEntryData* InEntryData);

	/** Called when the quick bar component has changed to a new quick bar */
	UFUNCTION(BlueprintImplementableEvent, Category = "Quick Bar")
	void OnQuickBarChanged(UBotaniQuickBarComponent* NewQuickBar, UBotaniQuickBarComponent* OldQuickBar);

	/** Returns the slot id of the entry data */
	UFUNCTION(BlueprintPure, Category = "Quick Bar")
	int32 GetQuickBarSlotID() const;

	/** Returns the current item definition of the entry data */
	UFUNCTION(BlueprintPure, Category = "Quick Bar")
	class UBotaniInventoryItemDefinition* GetItemDefinition() const;

	/** Returns the current item instance of the entry data */
	UFUNCTION(BlueprintPure, Category = "Quick Bar")
	class UBotaniItemInstance* GetItemInstance() const;

	/** Returns the current equipment instance of the entry data */
	UFUNCTION(BlueprintPure, Category = "Quick Bar")
	class UBotaniEquipmentInstance* GetEquipmentInstance() const;
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Quick Bar")
	TObjectPtr<UBotaniQuickBarComponent> QuickBar;

private:
	UPROPERTY(Transient)
	TObjectPtr<UBotaniQuickBarEntryData> EntryData;
};
