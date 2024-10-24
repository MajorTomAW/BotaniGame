// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Widgets/BotaniHUDLayout.h"

#include "CommonUIExtensions.h"
#include "GameplayTags/BotaniGameplayTags.h"
#include "GameplayTags/BotaniGameplayTags.h"
#include "Input/CommonUIInputTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniHUDLayout)

UBotaniHUDLayout::UBotaniHUDLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBotaniHUDLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterUIActionBinding(FBindUIActionArgs(
		FUIActionTag::ConvertChecked(BotaniGameplayTags::UI::TAG_UI_Action_Escape),
		false,
		FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEscapeAction)
		));
}

void UBotaniHUDLayout::HandleEscapeAction()
{
	if (!ensureMsgf(!EscapeMenuClass.IsNull(), TEXT("UBotaniHUDLayout::HandleEscapeAction: EscapeMenuClass is null!")))
	{
		return;
	}

	UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(GetOwningLocalPlayer(), BotaniGameplayTags::UI::TAG_UI_Layer_Menu, EscapeMenuClass);
}
