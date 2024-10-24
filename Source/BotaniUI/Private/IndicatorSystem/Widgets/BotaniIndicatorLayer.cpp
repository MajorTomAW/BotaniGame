// Copyright © 2024 Botanibots Team. All rights reserved.


#include "IndicatorSystem/Widgets/BotaniIndicatorLayer.h"

#include "IndicatorSystem/SActorCanvas.h"


UBotaniIndicatorLayer::UBotaniIndicatorLayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
	UWidget::SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UBotaniIndicatorLayer::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyActorCanvas.Reset();
}

TSharedRef<SWidget> UBotaniIndicatorLayer::RebuildWidget()
{
	if (!IsDesignTime())
	{
		ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();

		if (ensureMsgf(LocalPlayer, TEXT("Attempting to rebuild a UActorCanvas without a valid LocalPlayer!")))
		{
			MyActorCanvas = SNew(SActorCanvas, FLocalPlayerContext(LocalPlayer), &ArrowBrush);
			return MyActorCanvas.ToSharedRef();
		}
	}

	// Give it a trivial box, NullWidget isn't safe to use from a UWidget
	return SNew(SBox);
}
