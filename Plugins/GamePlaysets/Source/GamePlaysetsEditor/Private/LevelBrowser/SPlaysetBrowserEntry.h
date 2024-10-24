// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Slate/PlaysetConverter/SPlaysetConvertAssetWindow.h"
#include "Widgets/SCompoundWidget.h"

class SPlaysetBrowserEntry : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPlaysetBrowserEntry)
		: _ThumbnailSize(64.f)
		{
		}
		SLATE_ARGUMENT(float, ThumbnailSize)
		SLATE_EVENT(FOnVisualizeAssetToolTip, OnVisualizeAssetToolTip)
	SLATE_END_ARGS()

	using SSelf = SPlaysetBrowserEntry;

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedPtr<struct FPlaceablePlayset>& InItem);

	virtual bool OnVisualizeTooltip(const TSharedPtr<SWidget>& TooltipContent) override;
	virtual void OnToolTipClosing() override;
	virtual TSharedPtr<IToolTip> GetToolTip() override;
	virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;

	/** Get the name text to be displayed for this item. */
	FText GetNameText() const;

	/** Returns the border brush to use for this item. */
	const FSlateBrush* GetBorder() const;

protected:
	FOnVisualizeAssetToolTip OnVisualizeAssetToolTip;
	TSharedPtr<FPlaceablePlayset> Item;
	bool IsPressed() const { return bIsPressed; }

private:
	TSharedPtr<SWidget> AssetThumbnail;
	uint32 bIsPressed : 1 = false;
};
