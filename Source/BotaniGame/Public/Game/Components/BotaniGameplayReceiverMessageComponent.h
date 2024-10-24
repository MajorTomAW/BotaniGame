// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniGameplayMessageComponent.h"
#include "GameplayTagContainer.h"
#include "Messaging/BotaniVerbMessage.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "BotaniGameplayReceiverMessageComponent.generated.h"

struct FBotaniVerbMessage;

/**
 * @class UBotaniGameplayReceiverMessageComponent
 *
 * Base class for any message processor which observes other gameplay messages
 * and potentially re-emits updates (e.g., when a chain or combo is detected)
 *
 * @note These processors are spawned on the server once (not per player)
 * and should do their own internal filtering if only relevant for some players.
 */
UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent, ShortToolTip = "A component that listens for messages on specified channels", DisplayName = "Gameplay Receiver Message Component"))
class BOTANIGAME_API UBotaniGameplayReceiverMessageComponent : public UBotaniGameplayMessageComponent
{
	GENERATED_UCLASS_BODY()

public:
	/** Called when a message is received on a channel this component is listening to */
	UPROPERTY(BlueprintAssignable, meta = (Displayname = "On Message Received"))
	FOnBotaniGameplayMessageBPSignature OnMessageReceivedDelegate_BP;

public:
	/** Start listening to a specified channel, also adds the handle to the internal map */
	virtual void StartListeningToChannel(FGameplayTag ChannelId);
	virtual FGameplayMessageListenerHandle StartListeningToChannel_ReturnHandle(FGameplayTag ChannelId);

	/** Stop listening to a specified channel, also removes the handle from the internal map */
	virtual void StopListeningToChannel(FGameplayMessageListenerHandle Handle);
	virtual void StopListeningToChannel(FGameplayTag ChannelId);

	virtual void StartListening();
	virtual void StopListening();

	void BindOnReceivedDelegate(FOnBotaniGameplayMessageSignature::FDelegate&& Delegate);
	void UnbindOnReceivedDelegate(FDelegateHandle Handle);

protected:
	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent Interface

	/** Called when a message is received on a channel this component is listening to */
	virtual void OnMessageReceived(FGameplayTag Channel, const FBotaniVerbMessage& Payload);

	double GetServerTime() const;

	/** Called when a message is received on a channel this component is listening to */
	FOnBotaniGameplayMessageSignature OnMessageReceivedDelegate;

private:
	typedef TMap<FGameplayTag, FGameplayMessageListenerHandle> FChannelListenerMap;
	FChannelListenerMap ListenerHandles;
};
