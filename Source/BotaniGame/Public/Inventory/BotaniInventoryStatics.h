// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayInventoryTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BotaniInventoryStatics.generated.h"

class UBotaniQuickBarComponent;
struct FGameplayInventoryItemSpecHandle;
/**
 * 
 */
UCLASS()
class BOTANIGAME_API UBotaniInventoryStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FGameplayInventoryItemContext CreateItemContext(AController* InventoryOwner, UObject* Instigator, const FGameplayInventoryItemSpecHandle& ItemSpecHandle, int32 Quantity = 1);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory System", meta = (DefaultToSelf = "Actor"))
	static UBotaniQuickBarComponent* FindQuickBarComponent(const AActor* Actor);
};
