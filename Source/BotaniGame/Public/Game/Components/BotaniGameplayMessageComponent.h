// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/GameFrameworkComponent.h"
#include "BotaniGameplayMessageComponent.generated.h"

struct FBotaniVerbMessage;

/** Delegate for when a message is received on a channel this component is listening to */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBotaniGameplayMessageBPSignature, FGameplayTag, Channel, const FBotaniVerbMessage&, Payload);

/** Delegate for when a message is received on a channel this component is listening to */
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBotaniGameplayMessageSignature, FGameplayTag, const FBotaniVerbMessage&);

/**
 * @class UBotaniGameplayMessageComponent
 *
 * Base class for any gameplay message components that either send or receive gameplay messages.
 * @note These processors should only be spawned on the server one (not per player) and should do their own internal filtering if only relevant for some players.
 */
UCLASS(Abstract, Blueprintable, HideCategories = (AssetUserData, Cooking, Navigation, Tags))
class BOTANIGAME_API UBotaniGameplayMessageComponent : public UGameFrameworkComponent
{
	GENERATED_UCLASS_BODY()

public:
	/** Channel Ids to use for gameplay messages */
	UPROPERTY(EditDefaultsOnly, Category = "Channel", meta = (DisplayName = "Channel Ids", Categories = "Gameplay.Message"))
    FGameplayTagContainer ChannelIds;
};
