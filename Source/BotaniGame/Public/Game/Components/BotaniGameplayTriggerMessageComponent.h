// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniGameplayMessageComponent.h"
#include "BotaniGameplayTriggerMessageComponent.generated.h"

class UGameplayMessageSubsystem;

/**
 * @class UBotaniGameplayTriggerMessageComponent
 *
 * Base class for any message trigger components that send gameplay messages.
 * @note These processors should only be spawned on the server one (not per player) and should do their own internal filtering if only relevant for some players.
 */
UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent, ShortToolTip = "A component that sends messages on specified channels", DisplayName = "Gameplay Trigger Message Component"))
class BOTANIGAME_API UBotaniGameplayTriggerMessageComponent : public UBotaniGameplayMessageComponent
{
	GENERATED_UCLASS_BODY()

public:
	/** Called when a message is sent on a channel this component is listening to */
	UPROPERTY(BlueprintAssignable, meta = (Displayname = "On Message Sent"))
	FOnBotaniGameplayMessageBPSignature OnMessageSentDelegate_BP;

	/** Sends a gameplay message on the specified channel id, with a custom payload */
	UFUNCTION(BlueprintCallable, Category = "Botani|Messaging", meta = (Categories = "Gameplay.Message"))
	void SendMessageToChannelWithPayload(FGameplayTag ChannelId, const FBotaniVerbMessage& Payload);

	/** Sends a gameplay message to all channels, with a custom payload */
	UFUNCTION(BlueprintCallable, Category = "Botani|Messaging")
	void SendMessageToAllChannelsWithPayload(const FBotaniVerbMessage& Payload);
	

protected:
	UGameplayMessageSubsystem* GetMessageSubsystem();
	
	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent Interface

	/** Called when a message is sent on a channel this component is listening to */
	FOnBotaniGameplayMessageSignature OnMessageSentDelegate;

	/** Called to send a message to the specified channel */
	virtual void SendMessageToChannel_Internal(const FGameplayTag& ChannelId, const FBotaniVerbMessage& Payload);
	
private:
	UPROPERTY()
	TObjectPtr<UGameplayMessageSubsystem> MessageSubsystem;
};
