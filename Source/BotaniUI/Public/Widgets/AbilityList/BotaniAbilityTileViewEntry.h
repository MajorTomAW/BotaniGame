// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widgets/BotaniTaggedWidget.h"
#include "Widgets/BotaniUserWidget.h"
#include "BotaniAbilityTileViewEntry.generated.h"

class UGameplayAbility;
/**
 * UBot
 */
UCLASS()
class BOTANIUI_API UBotaniAbilityTileViewEntry : public UBotaniTaggedWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	/** Called when a gameplay tag changes. */
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void OnGameplayTagChanged(const FGameplayTag GameplayTag, int32 Count);

	/** Called when an ability is activated. */
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void OnAbilityActivated(UGameplayAbility* Ability);

	/** Called when an ability ends. */
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void OnAbilityEnded(UGameplayAbility* Ability);
};
