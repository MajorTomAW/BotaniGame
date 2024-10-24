// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "BotaniTaggedWidget.generated.h"

/**
 * UBotaniTaggedWidget
 *
 * A widget in a layout that has been tagged (can be hidden or shown via tags on the owning player)
 */
UCLASS(Abstract, Blueprintable)
class BOTANIUI_API UBotaniTaggedWidget : public UCommonUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	//~Begin UWidget interface
	virtual void SetVisibility(ESlateVisibility InVisibility) override;
	//~End UWidget interface

	//~Begin UUserWidget interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~End UUserWidget interface

protected:
	/** If the owning player has any of these tags, this widget will be hidden (using HiddenVisibility) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD")
	FGameplayTagContainer HiddenByTags;

	/** The visibility to use when this widget is shown (not hidden by gameplay tags). */
	UPROPERTY(EditAnywhere, Category = "HUD")
	ESlateVisibility ShownVisibility = ESlateVisibility::Visible;

	/** The visibility to use when this widget is hidden by gameplay tags. */
	UPROPERTY(EditAnywhere, Category = "HUD")
	ESlateVisibility HiddenVisibility = ESlateVisibility::Collapsed;

protected:
	/** Do we want to be visible (ignoring tags)? */
	bool bWantsToBeVisible = true;

private:
	void OnWatchedTagsChanged();
};
