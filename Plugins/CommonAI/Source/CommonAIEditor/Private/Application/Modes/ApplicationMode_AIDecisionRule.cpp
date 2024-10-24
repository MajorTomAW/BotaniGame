// Copyright © 2024 Botanibots Team. All rights reserved.


#include "ApplicationMode_AIDecisionRule.h"

#include "CommonAIEditorModule.h"
#include "Application/AIDecisionEditorApp.h"

FApplicationMode_AIDecisionRule::FApplicationMode_AIDecisionRule(TSharedPtr<class FAIDecisionEditorApp> InDecisionEditorApp)
	: FApplicationMode(FAIDecisionEditorApp::DecisionRuleMode, FAIDecisionEditorApp::GetLocalizedMode)
{
	AppPtr = InDecisionEditorApp;

	TabLayout = FTabManager::NewLayout("Standalone_AIDecisionRule_Layout_v2")
	->AddArea
	(
		FTabManager::NewPrimaryArea()
		->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewSplitter()
			->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(1.f)
				->AddTab(CommonAIEditor::GraphEditorID, ETabState::ClosedTab)
			)
		)
	);
}

void FApplicationMode_AIDecisionRule::RegisterTabFactories(TSharedPtr<class FTabManager> InTabManager)
{
	check(AppPtr.IsValid());
	TSharedPtr<FAIDecisionEditorApp> AppPtrPin = AppPtr.Pin();
	AppPtrPin->PushTabFactories(DecisionRuleTabFactories);
	
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FApplicationMode_AIDecisionRule::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void FApplicationMode_AIDecisionRule::PostActivateMode()
{
	check(AppPtr.IsValid());
	TSharedPtr<FAIDecisionEditorApp> App = AppPtr.Pin();
	App->RestoreDecisionRuleGraph();


	FApplicationMode::PostActivateMode();
}
