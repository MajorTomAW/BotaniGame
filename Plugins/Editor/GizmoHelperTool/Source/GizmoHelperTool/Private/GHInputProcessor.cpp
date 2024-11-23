// Copyright © 2024 Game Shifters Studio. All rights reserved.


#include "GHInputProcessor.h"

FGHInputProcessor::FGHInputProcessor() = default;
FGHInputProcessor::~FGHInputProcessor() = default;

bool FGHInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    OnKeyPressedDownDelegate.Broadcast(InKeyEvent.GetKey());
    return false;
}

bool FGHInputProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    OnKeyPressedUpDelegate.Broadcast(InKeyEvent.GetKey());
    return false;
}

bool FGHInputProcessor::HandleMouseButtonDownEvent( FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
    OnMouseButtonDownDelegate.Broadcast(MouseEvent.GetEffectingButton());
    return false;
}

bool FGHInputProcessor::HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
    OnMouseButtonUpDelegate.Broadcast(MouseEvent.GetEffectingButton());
    return false;
}

void FGHInputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
}

