// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Foundation/BotaniActionWidget.h"

#include "CommonInputBaseTypes.h"
#include "CommonInputSubsystem.h"
#include "EnhancedInputSubsystems.h"

UBotaniActionWidget::UBotaniActionWidget()
{
}

FSlateBrush UBotaniActionWidget::GetIcon() const
{
	if (!AssociatedInputAction)
	{
		return Super::GetIcon();
	}

	if (const UEnhancedInputLocalPlayerSubsystem* InputSub = GetEnhancedInputSubsystem())
	{
		TArray<FKey> BoundKeys = InputSub->QueryKeysMappedToAction(AssociatedInputAction);
		FSlateBrush SlateBrush;

		const UCommonInputSubsystem* CommonInputSub = GetInputSubsystem();
		if (!BoundKeys.IsEmpty() && CommonInputSub && UCommonInputPlatformSettings::Get()->TryGetInputBrush(SlateBrush, BoundKeys[0], CommonInputSub->GetCurrentInputType(), CommonInputSub->GetCurrentGamepadName()))
		{
			return SlateBrush;
		}
	}

	return Super::GetIcon();
}

UEnhancedInputLocalPlayerSubsystem* UBotaniActionWidget::GetEnhancedInputSubsystem() const
{
	const UWidget* BoundWidget = DisplayedBindingHandle.GetBoundWidget();
	if (const ULocalPlayer* BindingOwner = BoundWidget ? BoundWidget->GetOwningLocalPlayer() : GetOwningLocalPlayer())
	{
		return BindingOwner->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	}
	return nullptr;
}
