// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Widgets/BotaniDraggedWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniDraggedWidget)

UBotaniDraggedWidget* UBotaniDragDropOperation::GetDraggedWidget() const
{
	return DefaultDragVisual ? Cast<UBotaniDraggedWidget>(DefaultDragVisual) : nullptr;	
}

void UBotaniDragDropOperation::UpdateDragDropResponse(EBotaniDragDropResponse Response, const FText& Message)
{
	if (UBotaniDraggedWidget* DraggedWidget = GetDraggedWidget())
	{
		DraggedWidget->UpdateDragDropResponse(Response, Message);
	}
}

UObject* UBotaniDragDropOperation::GetTypedPayload(const TSubclassOf<UObject> PayloadType)
{
	if (Payload && Payload->IsA(PayloadType))
	{
		return Payload;
	}

	return nullptr;
}


UBotaniDraggedWidget::UBotaniDraggedWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBotaniDraggedWidget::NativeOnDraggedWidgetCreated(UBotaniDragDropOperation* InOperation)
{
	OnDraggedWidgetCreated(InOperation);
}

void UBotaniDraggedWidget::UpdateDragDropResponse_Implementation(EBotaniDragDropResponse Response, const FText& Message)
{
}
