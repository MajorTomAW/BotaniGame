// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIGraphEditor.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class FDocumentTabFactory;
class FDocumentTracker;

class FAIDecisionEditorApp : public FWorkflowCentricApplication, public FAIGraphEditor, public FNotifyHook
{
public:
	FAIDecisionEditorApp();
	virtual void InitDecisionEditorApp(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* InObject);

	//~ Begin FWorkflowCentricApplication Interface
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FString GetDocumentationLink() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	//~ End FWorkflowCentricApplication Interface

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;

public:
	static const FName DecisionRuleMode;

	/** Get the localized name of the mode */
	static FText GetLocalizedMode(FName InMode);
	
	/** Create widget for graph editing */
	TSharedRef<class SGraphEditor> CreateGraphEditorWidget(UEdGraph* InGraph);

	void RestoreDecisionRuleGraph();

private:
	/** The AI decision rule being edited */
	class UAIDecisionRule* DecisionRule;

	TSharedPtr<FDocumentTracker> DocumentManager;
	TWeakPtr<FDocumentTabFactory> GraphEditorTabFactoryPtr;
};
