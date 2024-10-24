// Copyright 2024 MajorT. All rights reserved.

#include "CommonAIEditorModule.h"

#include "Application/AIDecisionEditorApp.h"

#define LOCTEXT_NAMESPACE "FCommonAIEditorModule"

const FName ICommonAIEditorModule::AIDecisionEditorAppIdentifier(TEXT("AIDecisionRuleEditorApp"));

class FCommonAIEditorModule final : public ICommonAIEditorModule
{
public:
	//~ Begin IModuleInterface Interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface Interface

	virtual TSharedRef<FWorkflowCentricApplication> CreateAIDecisionEditorApp(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* Object) override;
	
};
IMPLEMENT_MODULE(FCommonAIEditorModule, CommonAIEditor)


void FCommonAIEditorModule::StartupModule()
{
}

void FCommonAIEditorModule::ShutdownModule()
{
}

TSharedRef<FWorkflowCentricApplication> FCommonAIEditorModule::CreateAIDecisionEditorApp(
	const EToolkitMode::Type Mode, 	const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* Object)
{
	TSharedRef<FAIDecisionEditorApp> App(new FAIDecisionEditorApp());
	App->InitDecisionEditorApp(Mode, InitToolkitHost, Object);
	return App;
}


#undef LOCTEXT_NAMESPACE
