// Copyright © 2024 Botanibots Team. All rights reserved.


#include "DRGraphEditorSummoner.h"

#include "CommonAIEditorModule.h"
#include "Application/AIDecisionEditorApp.h"

FDRGraphEditorSummoner::FDRGraphEditorSummoner(
	TSharedPtr<class FAIDecisionEditorApp> InApp, FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback)
	: FDocumentTabFactoryForObjects<UEdGraph>(CommonAIEditor::GraphEditorID, InApp)
	, AppPtr(InApp)
	, CreateGraphEditorWidget(CreateGraphEditorWidgetCallback) 
{
}

void FDRGraphEditorSummoner::OnTabActivated(TSharedPtr<SDockTab> Tab) const
{
	check(AppPtr.IsValid());
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
}

void FDRGraphEditorSummoner::OnTabRefreshed(TSharedPtr<SDockTab> Tab) const
{
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	GraphEditor->NotifyGraphChanged();
}

TAttribute<FText> FDRGraphEditorSummoner::ConstructTabNameForObject(UEdGraph* DocumentID) const
{
	return TAttribute<FText>(FText::FromString(DocumentID->GetName()));
}

TSharedRef<SWidget> FDRGraphEditorSummoner::CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
	return CreateGraphEditorWidget.Execute(DocumentID);
}

const FSlateBrush* FDRGraphEditorSummoner::GetTabIconForObject(
	const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
	return FAppStyle::Get().GetBrush("ClassIcon.BehaviorTree");
}

void FDRGraphEditorSummoner::SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const
{
	FDocumentTabFactoryForObjects<UEdGraph>::SaveState(Tab, Payload);
}
