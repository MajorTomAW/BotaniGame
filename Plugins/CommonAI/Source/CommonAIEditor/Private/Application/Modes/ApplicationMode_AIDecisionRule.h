// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

/** Application mode for editing AI decision rules */
class FApplicationMode_AIDecisionRule final : public FApplicationMode
{
public:
	FApplicationMode_AIDecisionRule(TSharedPtr<class FAIDecisionEditorApp> InDecisionEditorApp);

	//~ Begin FApplicationMode Interface
	virtual void RegisterTabFactories(TSharedPtr<class FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;
	//~ End FApplicationMode Interface

protected:
	TWeakPtr<FAIDecisionEditorApp> AppPtr;
	FWorkflowAllowedTabSet DecisionRuleTabFactories;
};