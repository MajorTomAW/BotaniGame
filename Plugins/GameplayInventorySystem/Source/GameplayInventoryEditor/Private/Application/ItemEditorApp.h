// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class FItemEditorApp : public FWorkflowCentricApplication, public FNotifyHook, public FEditorUndoClient
{
public:
	FItemEditorApp();

	virtual void InitItemEditorApp(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* Object);

public:
	//~ Begin FAssetEditorToolkit Interface
	FORCEINLINE virtual FName GetToolkitFName() const override { return FName("RuleSetEditor"); }
	FORCEINLINE virtual FText GetBaseToolkitName() const override { return NSLOCTEXT("RuleSetEditorApp", "DisplayName", "Rule Set Editor"); }
	FORCEINLINE virtual FText GetToolkitName() const override;
	FORCEINLINE virtual FString GetWorldCentricTabPrefix() const override { return NSLOCTEXT("RuleSetEditorApp", "WorldCentricTabPrefix", "Rule Set Editor").ToString(); }
	FORCEINLINE virtual FString GetDocumentationLink() const override { return TEXT(""); }
	FORCEINLINE virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::White; }

	virtual void OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit) override;
	virtual void OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit) override;
	virtual void SetCurrentMode(FName NewMode) override;
	virtual FText GetToolkitToolTipText() const override;

	virtual void SaveAsset_Execute() override;
	//~ End FAssetEditorToolkit Interface

	bool CanSwitchToItemMode() const;
	bool CanSwitchToEquipmentMode() const;

	TSharedPtr<IDetailsView> GetDetailsView() const;
	TSharedPtr<IDetailsView> GetFragmentDetailsView() const;
	class UGameplayInventoryItemDefinition* GetItemDefinition() const { return ItemDefinition; }
	class UGameplayEquipmentDefinition* GetEquipmentDefinition() const;
	TSharedPtr<class FItemEditorToolbar> GetToolbarBuilder() const { return ToolbarBuilder; }

protected:
	/** Property View */
	TSharedPtr<class IDetailsView> DetailsView;
	TSharedPtr<class IDetailsView> FragmentDetailsView;

	class UGameplayInventoryItemDefinition* ItemDefinition;
	class UGameplayEquipmentDefinition* EquipmentDefinition;

	TSharedPtr<class FItemEditorToolbar> ToolbarBuilder;
};