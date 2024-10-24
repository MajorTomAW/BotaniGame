// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class FItemEditorApp;

class FApplicationMode_Equipment : public FApplicationMode
{
public:
	FApplicationMode_Equipment(TSharedPtr<FItemEditorApp> App);

	//~ Begin FApplicationMode Interface
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;
	//~ End FApplicationMode Interface

private:
	TWeakPtr<FItemEditorApp> AppPtr;
	FWorkflowAllowedTabSet TabFactories;
};
