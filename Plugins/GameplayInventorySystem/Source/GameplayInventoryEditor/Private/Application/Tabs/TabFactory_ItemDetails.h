// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class FTabFactory_ItemDetails : public FWorkflowTabFactory
{
public:
	FTabFactory_ItemDetails(const TSharedPtr<class FItemEditorApp>& App);

	//~ Begin FWorkflowTabFactory Interface
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	//~ End FWorkflowTabFactory Interface

protected:
	TWeakPtr<class FItemEditorApp> AppPtr;
};