// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once


class FItemEditorApp;

class FItemEditorToolbar : public TSharedFromThis<FItemEditorToolbar>
{
public:
	FItemEditorToolbar(TSharedPtr<FItemEditorApp> InApp)
		: AppPtr(InApp)
	{
	}

	void AddModesToolbar(TSharedPtr<FExtender> Extender);
	void AddEditorToolbar(TSharedPtr<FExtender> Extender);


protected:
	void FillModesToolbar(FToolBarBuilder& ToolbarBuilder);

protected:
	TWeakPtr<FItemEditorApp> AppPtr;
	bool bCreateActionsEnabled = true;
};
