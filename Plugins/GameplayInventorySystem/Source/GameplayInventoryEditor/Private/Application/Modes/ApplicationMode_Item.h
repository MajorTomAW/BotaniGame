// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class FItemEditorApp;

class FApplicationMode_Item : public FApplicationMode
{
public:
	FApplicationMode_Item(TSharedPtr<FItemEditorApp> App);

	//~ Begin FApplicationMode Interface
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;
	//~ End FApplicationMode Interface

private:
	TWeakPtr<FItemEditorApp> AppPtr;
	FWorkflowAllowedTabSet TabFactories;
};
