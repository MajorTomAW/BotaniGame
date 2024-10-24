// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SNewPlaysetPicker.h"
#include "Widgets/SWindow.h"


class GAMEPLAYSETSEDITOR_API SNewPlaysetPickerWindow : public SWindow
{
public:
	SLATE_BEGIN_ARGS(SNewPlaysetPickerWindow)
		{
		}
		SLATE_ARGUMENT(SNewPlaysetPicker::FArguments, NewPlaysetPickerArgs)
		SLATE_ARGUMENT(TOptional<FText>, WindowTitle)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	bool ShouldProceedWithAction() const { return bProceedWithAction; }
	TWeakObjectPtr<UClass> GetSelectedClass() const { return SelectedClass; }

protected:
	TSharedPtr<SNewPlaysetPicker> NewPlaysetPicker;

	EVisibility GetSelectButtonVisibility() const;

	FReply OnSelectButtonClicked();
	FReply OnCancelButtonClicked();

private:
	void OnClassPicked(UClass* InChosenClass);


	TWeakObjectPtr<UClass> SelectedClass;
	bool bProceedWithAction = false;
};
