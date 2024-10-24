// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Widgets/BotaniTaggedWidget.h"

UBotaniTaggedWidget::UBotaniTaggedWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBotaniTaggedWidget::SetVisibility(ESlateVisibility InVisibility)
{
#if WITH_EDITORONLY_DATA
	if (IsDesignTime())
	{
		Super::SetVisibility(InVisibility);
		return;
	}
#endif

	bWantsToBeVisible = ConvertSerializedVisibilityToRuntime(InVisibility).IsVisible();
	if (bWantsToBeVisible)
	{
		ShownVisibility = InVisibility;
	}
	else
	{
		HiddenVisibility = InVisibility;
	}

	const bool bHasHiddenTags = false;//@TODO: Foo->HasAnyTags(HiddenByTags);

	// Apply the visibility
	const ESlateVisibility DesiredVisibility = (bWantsToBeVisible && !bHasHiddenTags) ? ShownVisibility : HiddenVisibility;
	if (GetVisibility() != DesiredVisibility)
	{
		Super::SetVisibility(DesiredVisibility);
	}
}

void UBotaniTaggedWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!IsDesignTime())
	{
		//@TODO: Listen for tag changes on our hidden tags
		
		// Set our initial visibility value (checking the tags, etc...)
		SetVisibility(GetVisibility());
	}
}

void UBotaniTaggedWidget::NativeDestruct()
{
	if (!IsDesignTime())
	{
		//@TODO: Stop listening for tag changes
	}
	
	Super::NativeDestruct();
}

void UBotaniTaggedWidget::OnWatchedTagsChanged()
{
	const bool bHasHiddenTags = false;//@TODO: Foo->HasAnyTags(HiddenByTags);

	// Actually apply the visibility
	const ESlateVisibility DesiredVisibility = (bWantsToBeVisible && !bHasHiddenTags) ? ShownVisibility : HiddenVisibility;
	if (GetVisibility() != DesiredVisibility)
	{
		Super::SetVisibility(DesiredVisibility);
	}
}
