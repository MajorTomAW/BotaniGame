// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Widgets/BotaniUserWidget.h"

#include "Character/BotaniCharacter.h"
#include "Player/BotaniLocalPlayer.h"
#include "Player/BotaniPlayerController.h"

UBotaniUserWidget::UBotaniUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

ABotaniPlayerController* UBotaniUserWidget::GetOwningBotaniPlayer() const
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		return Cast<ABotaniPlayerController>(PC);
	}

	return nullptr;
}

ABotaniCharacter* UBotaniUserWidget::GetOwningBotaniCharacter() const
{
	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		return Cast<ABotaniCharacter>(Pawn);
	}

	return nullptr;
}

UBotaniLocalPlayer* UBotaniUserWidget::GetOwningBotaniLocalPlayer() const
{
	if (ULocalPlayer* LocalPlayer = GetOwningLocalPlayer())
	{
		return Cast<UBotaniLocalPlayer>(LocalPlayer);
	}

	return nullptr;
}
