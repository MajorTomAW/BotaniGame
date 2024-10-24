// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Game/Components/BotaniGameplayReceiverMessageComponent.h"

#include "BotaniLogChannels.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/GameStateBase.h"

UBotaniGameplayReceiverMessageComponent::UBotaniGameplayReceiverMessageComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBotaniGameplayReceiverMessageComponent::StartListeningToChannel(FGameplayTag ChannelId)
{
	if (!UGameplayMessageSubsystem::HasInstance(GetWorld()))
	{
		BOTANI_LOG(Error, TEXT("GameplayMessageSubsystem not found in world!"));
		return;
	}

	UGameplayMessageSubsystem& Router = UGameplayMessageSubsystem::Get(GetWorld());
	FGameplayMessageListenerHandle NewHandle = Router.RegisterListener(ChannelId, this, &ThisClass::OnMessageReceived);

	check(NewHandle.IsValid());

	ListenerHandles.Add(ChannelId, NewHandle);
}

FGameplayMessageListenerHandle UBotaniGameplayReceiverMessageComponent::StartListeningToChannel_ReturnHandle(FGameplayTag ChannelId)
{
	StartListeningToChannel(ChannelId);
	return ListenerHandles[ChannelId];
}

void UBotaniGameplayReceiverMessageComponent::StopListeningToChannel(FGameplayTag ChannelId)
{
	if (!UGameplayMessageSubsystem::HasInstance(GetWorld()))
	{
		BOTANI_LOG(Error, TEXT("GameplayMessageSubsystem not found in world!"));
		return;
	}

	UGameplayMessageSubsystem& Router = UGameplayMessageSubsystem::Get(GetWorld());
	if (const FGameplayMessageListenerHandle* Handle = ListenerHandles.Find(ChannelId))
	{
		Router.UnregisterListener(*Handle);
		ListenerHandles.Remove(ChannelId);
	}
}

void UBotaniGameplayReceiverMessageComponent::StartListening()
{
}

void UBotaniGameplayReceiverMessageComponent::StopListening()
{
}

void UBotaniGameplayReceiverMessageComponent::BindOnReceivedDelegate(FOnBotaniGameplayMessageSignature::FDelegate&& Delegate)
{
	OnMessageReceivedDelegate.Add(MoveTemp(Delegate));
}

void UBotaniGameplayReceiverMessageComponent::UnbindOnReceivedDelegate(const FDelegateHandle Handle)
{
	OnMessageReceivedDelegate.Remove(Handle);
}

void UBotaniGameplayReceiverMessageComponent::StopListeningToChannel(const FGameplayMessageListenerHandle Handle)
{
	if (!UGameplayMessageSubsystem::HasInstance(GetWorld()))
	{
		BOTANI_LOG(Error, TEXT("[%s]: GameplayMessageSubsystem not found in world!"), *GetNameSafe(GetOwner()));
		return;
	}

	UGameplayMessageSubsystem& Router = UGameplayMessageSubsystem::Get(GetWorld());
	Router.UnregisterListener(Handle);
}

void UBotaniGameplayReceiverMessageComponent::BeginPlay()
{
	Super::BeginPlay();

	for (const auto& Channel : ChannelIds)
	{
		StartListeningToChannel(Channel);
	}

	StartListening();
}

void UBotaniGameplayReceiverMessageComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TArray<FGameplayMessageListenerHandle> HandlesArray;
	ListenerHandles.GenerateValueArray(HandlesArray);
	for (const auto& Handle : HandlesArray)
	{
		StopListeningToChannel(Handle);
	}

	ListenerHandles.Empty();

	StopListening();
	
	Super::EndPlay(EndPlayReason);
}

void UBotaniGameplayReceiverMessageComponent::OnMessageReceived(FGameplayTag Channel, const FBotaniVerbMessage& Payload)
{
	if (!Channel.IsValid())
	{
		BOTANI_LOG(Error, TEXT("[%s]: Received message on invalid channel!"), *GetNameSafe(GetOwner()));
	}

	OnMessageReceivedDelegate.Broadcast(Channel, Payload);
	OnMessageReceivedDelegate_BP.Broadcast(Channel, Payload);
}

double UBotaniGameplayReceiverMessageComponent::GetServerTime() const
{
	if (const AGameStateBase* GameState = GetWorld()->GetGameState())
	{
		return GameState->GetServerWorldTimeSeconds();
	}

	return 0.0f;
}


