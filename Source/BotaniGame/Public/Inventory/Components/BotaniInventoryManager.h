// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameplayInventoryManager.h"
#include "BotaniInventoryManager.generated.h"

/**
 * UBotaniInventoryManager
 *
 * An actor component class.
 */
UCLASS()
class BOTANIGAME_API UBotaniInventoryManager : public UGameplayInventoryManager
{
	GENERATED_UCLASS_BODY()

public:
	static UBotaniInventoryManager* GetInventoryManager(AActor* Actor);
	
	//~ Begin UGameplayInventoryManager Interface
	virtual void OnRemoveItem(FGameplayInventoryItemSpec& InSpec) override;
	//~ End UGameplayInventoryManager Interface
	
protected:
	static void OnShowInventoryDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos); 
private:
};
