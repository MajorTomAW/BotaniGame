// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AIDecisionEditorApp.h"

#include "AIDecisionRule.h"
#include "AIGraph.h"
#include "AIGraphSchema.h"
#include "CommonAIEditorModule.h"
#include "GraphEditorActions.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Modes/ApplicationMode_AIDecisionRule.h"
#include "Tabs/DRGraphEditorSummoner.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

#define LOCTEXT_NAMESPACE "FCommonAIEditorModule"

const FName FAIDecisionEditorApp::DecisionRuleMode = FName("DecisionRuleMode");

FAIDecisionEditorApp::FAIDecisionEditorApp()
{
	DecisionRule = nullptr;
}

void FAIDecisionEditorApp::InitDecisionEditorApp(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* InObject)
{
	UAIDecisionRule* DecisionRuleToEdit = Cast<UAIDecisionRule>(InObject);
	if (DecisionRuleToEdit)
	{
		DecisionRule = DecisionRuleToEdit;
	}

	TSharedPtr<FAIDecisionEditorApp> ThisPtr(SharedThis(this));
	if (!DocumentManager.IsValid())
	{
		DocumentManager = MakeShareable(new FDocumentTracker);
		DocumentManager->Initialize(ThisPtr);

		// Register document factories
		TSharedRef<FDocumentTabFactory> GraphEditorFactory = MakeShareable(new FDRGraphEditorSummoner(ThisPtr,
			FDRGraphEditorSummoner::FOnCreateGraphEditorWidget::CreateSP(this, &FAIDecisionEditorApp::CreateGraphEditorWidget)));

		// Cache tab factories
		GraphEditorTabFactoryPtr = GraphEditorFactory;
		DocumentManager->RegisterDocumentFactory(GraphEditorFactory);
	}

	// Get objects to edit
	TArray<UObject*> ObjectsToEdit;
	ObjectsToEdit.Add(DecisionRule);

	// If we are already editing objects, don't try to recreate the editor from scratch but update the list of objects
	const TArray<UObject*>* EditedObjects = GetObjectsCurrentlyBeingEdited();
	if (EditedObjects == nullptr || EditedObjects->Num() == 0)
	{
		FGraphEditorCommands::Register();

		constexpr bool bCreateDefaultStandaloneMenu = true;
		constexpr bool bCreateDefaultToolbar = true;
		InitAssetEditor(Mode, InitToolkitHost, ICommonAIEditorModule::AIDecisionEditorAppIdentifier, FTabManager::FLayout::NullLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ObjectsToEdit);

		AddApplicationMode(DecisionRuleMode, MakeShareable(new FApplicationMode_AIDecisionRule(SharedThis(this))));
	}
	else
	{
		for (UObject* ObjectToEdit : ObjectsToEdit)
		{
			if (!EditedObjects->Contains(ObjectToEdit))
			{
				AddEditingObject(ObjectToEdit);
			}
		}
	}

	// Set the initial mode
	if (DecisionRuleToEdit != nullptr)
	{
		SetCurrentMode(DecisionRuleMode);
	}

	OnClassListUpdated();
	RegenerateMenusAndToolbars();
}

FName FAIDecisionEditorApp::GetToolkitFName() const
{
	return "AIDecisionEditor";
}

FText FAIDecisionEditorApp::GetBaseToolkitName() const
{
	return LOCTEXT("AIDecisionEditorAppLabel", "AI Decision Editor");
}

FText FAIDecisionEditorApp::GetToolkitName() const
{
	return FWorkflowCentricApplication::GetToolkitName();
}

FText FAIDecisionEditorApp::GetToolkitToolTipText() const
{
	return FWorkflowCentricApplication::GetToolkitToolTipText();
}

FString FAIDecisionEditorApp::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "AI Decision Editor").ToString();
}

FString FAIDecisionEditorApp::GetDocumentationLink() const
{
	return FWorkflowCentricApplication::GetDocumentationLink();
}

FLinearColor FAIDecisionEditorApp::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

void FAIDecisionEditorApp::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	DocumentManager->SetTabManager(InTabManager);
	
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}

FText FAIDecisionEditorApp::GetLocalizedMode(FName InMode)
{
	static TMap<FName, FText> LocNodes;
	if (LocNodes.Num() == 0)
	{
		LocNodes.Add(DecisionRuleMode, LOCTEXT("DecisionRuleMode", "Decision Rule"));
	}

	check(InMode != NAME_None);
	const FText* OutDesc = LocNodes.Find(InMode);
	check(OutDesc);
	return *OutDesc;
}

void FAIDecisionEditorApp::RestoreDecisionRuleGraph()
{
	const TSubclassOf<UEdGraphSchema> SchemaClass = UAIGraphSchema::StaticClass();
	check(SchemaClass);

	UEdGraph* Graph = FBlueprintEditorUtils::CreateNewGraph(DecisionRule, "Decision Rule Graph", UAIGraph::StaticClass(), SchemaClass);
	UAIGraph* AIGraph = Cast<UAIGraph>(Graph);
	AIGraph->OnCreated();
	AIGraph->Initialize();

	TSharedRef<FTabPayload_UObject> Payload = FTabPayload_UObject::Make(Graph);
	TSharedPtr<SDockTab> DocumentTab = DocumentManager->OpenDocument(Payload, FDocumentTracker::RestorePreviousDocument);
}

TSharedRef<class SGraphEditor> FAIDecisionEditorApp::CreateGraphEditorWidget(UEdGraph* InGraph)
{
	check(InGraph != nullptr);

	if (!GraphEditorCommands.IsValid())
	{
		CreateCommandList();
	}

	// Make title bar
	TSharedRef<SWidget> TitleBarWidget = 
		SNew(SBorder)
		.BorderImage( FAppStyle::GetBrush( TEXT("Graph.TitleBackground") ) )
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.FillWidth(1.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("TitleBarText", "Decision Rule Editor Title"))
				.TextStyle( FAppStyle::Get(), TEXT("GraphBreadcrumbButtonText") )
			]
		];

	// Make full graph editor widget
	const bool bGraphIsEditable = InGraph->bEditable;
	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(bGraphIsEditable)
		.TitleBar(TitleBarWidget)
		.GraphToEdit(InGraph)
		.AutoExpandActionMenu(true);
}

#undef LOCTEXT_NAMESPACE
