// Copyright 2022 Ares9323. All Rights Reserved.

#include "RestartEditorCommands.h"

#define LOCTEXT_NAMESPACE "FRestartEditorModule"

void FRestartEditorCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "RestartEditor", "Restart Editor", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control|EModifierKey::Shift, EKeys::R));
}

#undef LOCTEXT_NAMESPACE
