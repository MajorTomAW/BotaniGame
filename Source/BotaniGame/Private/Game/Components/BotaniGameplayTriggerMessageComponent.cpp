// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Game/Components/BotaniGameplayTriggerMessageComponent.h"

#include "BotaniLogChannels.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Messaging/BotaniVerbMessage.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniGameplayTriggerMessageComponent)

UBotaniGameplayTriggerMessageComponent::UBotaniGameplayTriggerMessageComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBotaniGameplayTriggerMessageComponent::SendMessageToChannelWithPayload(FGameplayTag ChannelId, const FBotaniVerbMessage& Payload)
{
	SendMessageToChannel_Internal(ChannelId, Payload);
}

void UBotaniGameplayTriggerMessageComponent::SendMessageToAllChannelsWithPayload(const FBotaniVerbMessage& Payload)
{
	for (const FGameplayTag& ChannelId : ChannelIds)
	{
		SendMessageToChannel_Internal(ChannelId, Payload);
	}
}


UGameplayMessageSubsystem* UBotaniGameplayTriggerMessageComponent::GetMessageSubsystem()
{
	if (MessageSubsystem == nullptr)
	{
		MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	}
	
	return MessageSubsystem;
}

void UBotaniGameplayTriggerMessageComponent::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		MessageSubsystem = UGameplayMessageSubsystem::Get(World);
	}
	else
	{
		BOTANI_LOG(Error, TEXT("No world found for UBotaniGameplayTriggerMessageComponent"));
	}
}

void UBotaniGameplayTriggerMessageComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	MessageSubsystem = nullptr;
}

void UBotaniGameplayTriggerMessageComponent::SendMessageToChannel_Internal(const FGameplayTag& ChannelId, const FBotaniVerbMessage& Payload)
{
	UGameplayMessageSubsystem& MessageSub = *GetMessageSubsystem();
	MessageSub.BroadcastMessage(ChannelId, Payload);

	if (OnMessageSentDelegate.IsBound())
	{
		OnMessageSentDelegate.Broadcast(ChannelId, Payload);
	}

	if (OnMessageSentDelegate_BP.IsBound())
	{
		OnMessageSentDelegate_BP.Broadcast(ChannelId, Payload);
	}
}
