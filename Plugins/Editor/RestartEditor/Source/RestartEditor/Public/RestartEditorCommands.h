// Copyright 2022 Ares9323. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "RestartEditorStyle.h"

class FRestartEditorCommands : public TCommands<FRestartEditorCommands>
{
public:

	FRestartEditorCommands()
		: TCommands<FRestartEditorCommands>(TEXT("RestartEditor"), NSLOCTEXT("Contexts", "RestartEditor", "RestartEditor Plugin"), NAME_None, FRestartEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
