// Copyright © 2024 Game Shifters Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"
#include "GHInputProcessor.generated.h"


/**
 * Delegate to be called when a key event is received
 */
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyEventSignature, FKey, Key);


class GIZMOHELPERTOOL_API FGHInputProcessor : public IInputProcessor
{
public:
	FGHInputProcessor();
	virtual ~FGHInputProcessor() override;

	//~ Begin IInputProcessor Interface
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleMouseButtonDownEvent( FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;
	//~ End IInputProcessor Interface
	
	FOnKeyEventSignature OnKeyPressedDownDelegate;
	FOnKeyEventSignature OnKeyPressedUpDelegate;
	FOnKeyEventSignature OnMouseButtonDownDelegate;
	FOnKeyEventSignature OnMouseButtonUpDelegate;
};
