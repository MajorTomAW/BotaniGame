// Copyright © 2024 Botanibots Team. All rights reserved.


#include "SPlaysetBrowserEntry.h"

#include "ContentBrowserMenuContexts.h"
#include "Playset.h"
#include "PlaysetEditorIDs.h"
#include "SlateOptMacros.h"
#include "DragAndDrop/AssetDragDropOp.h"

#define LOCTEXT_NAMESPACE "SPlaysetBrowserEntry"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPlaysetBrowserEntry::Construct(const FArguments& InArgs, const TSharedPtr<FPlaceablePlayset>& InItem)
{
	Item = InItem;
	OnVisualizeAssetToolTip = InArgs._OnVisualizeAssetToolTip;

	// Asset thumbnail
	TSharedPtr<FAssetThumbnailPool> ThumbnailPool = UThumbnailManager::Get().GetSharedThumbnailPool();
	TSharedPtr<FAssetThumbnail> Thumbnail =
		MakeShareable(new FAssetThumbnail(
			Item->AssetData.GetAsset(),
			InArgs._ThumbnailSize,
			InArgs._ThumbnailSize,
			ThumbnailPool));
	FAssetThumbnailConfig Config;
	Config.bForceGenericThumbnail = false;
	Config.GenericThumbnailSize = InArgs._ThumbnailSize;
	Config.bAllowFadeIn = true;
	Config.bAllowHintText = true;
	AssetThumbnail = Thumbnail->MakeThumbnailWidget(Config);

	// Layout
	ChildSlot
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SNew(SBorder)
		.Cursor(EMouseCursor::GrabHand)
		.Padding(FMargin(0.f, 3.f))
		.ToolTip(GetToolTip())
		.BorderImage(this, &SSelf::GetBorder)
		[
			SNew(SVerticalBox)

			// Thumbnail
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			[
				SNew(SBox)
				.MaxDesiredHeight(InArgs._ThumbnailSize)
				.MaxDesiredWidth(InArgs._ThumbnailSize)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("ContentBrowser.ThumbnailShadow"))
					.ForegroundColor(FLinearColor::White)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Top)
					[
						AssetThumbnail.ToSharedRef()
					]
				]
			]

			// Display Name
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.Padding(FMargin(0.f, 4.f, 0.f, 0.f))
			[
				SNew(STextBlock)
				.Justification(ETextJustify::Center)
				.Font(FAppStyle::GetFontStyle("NormalFontBold"))
				.Text(GetNameText())
			]
		]
	];
}

bool SPlaysetBrowserEntry::OnVisualizeTooltip(const TSharedPtr<SWidget>& TooltipContent)
{
	TSharedPtr<IToolTip> ThisTooltip = GetToolTip();
	if (!ThisTooltip.IsValid() || (ThisTooltip->AsWidget() != TooltipContent))
	{
		return false;
	}

	if (OnVisualizeAssetToolTip.IsBound() && TooltipContent.IsValid() && Item.IsValid())
	{
		FAssetData ItemAssetData = Item->AssetData;
		return OnVisualizeAssetToolTip.Execute(TooltipContent, ItemAssetData);
	}

	return false;
}

void SPlaysetBrowserEntry::OnToolTipClosing()
{
	SCompoundWidget::OnToolTipClosing();
}

TSharedPtr<IToolTip> SPlaysetBrowserEntry::GetToolTip()
{
	if (!Item.IsValid())
	{
		return nullptr;
	}

	const UPlayset* Playset = Cast<UPlayset>(Item->AssetData.GetAsset());

	// Get the description text
	FText NameText = Playset->DisplayInfo.ItemDescription;
	if (NameText.IsEmpty())
	{
		NameText = Playset->DisplayInfo.ItemShortDescription;
	}
	if (NameText.IsEmpty())
	{
		NameText = Item->DisplayName;
	}

	// Add the path to the tooltip
	NameText = FText::Format(LOCTEXT("PlaysetBrowserEntryToolTip", "{0}\n\nPath: {1}"), NameText, FText::FromString(Item->AssetData.GetSoftObjectPath().ToString()));
	return FSlateApplication::Get().MakeToolTip(NameText);
}

void SPlaysetBrowserEntry::OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	SCompoundWidget::OnDragEnter(MyGeometry, DragDropEvent);
}

void SPlaysetBrowserEntry::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
	SCompoundWidget::OnDragLeave(DragDropEvent);
}

FReply SPlaysetBrowserEntry::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	return SCompoundWidget::OnDragOver(MyGeometry, DragDropEvent);
}

FReply SPlaysetBrowserEntry::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	return SCompoundWidget::OnDrop(MyGeometry, DragDropEvent);
}

FReply SPlaysetBrowserEntry::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bIsPressed = false;
	if (FEditorDelegates::OnAssetDragStarted.IsBound())
	{
		TArray<FAssetData> DraggedAssets;
		DraggedAssets.Add(Item->AssetData);

		FEditorDelegates::OnAssetDragStarted.Broadcast(DraggedAssets, Item->Factory);
		return FReply::Handled();
	}

	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return FReply::Handled().BeginDragDrop(FAssetDragDropOp::New(Item->AssetData, Item->Factory));
	}

	return FReply::Handled();
}

FReply SPlaysetBrowserEntry::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsPressed = true;

		return FReply::Handled().DetectDrag(SharedThis(this), MouseEvent.GetEffectingButton());
	}
	
	return SCompoundWidget::OnMouseButtonDown(MyGeometry, MouseEvent);
}

FReply SPlaysetBrowserEntry::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsPressed = false;
		return FReply::Handled();
	}

	return SCompoundWidget::OnMouseButtonUp(MyGeometry, MouseEvent);
}

FReply SPlaysetBrowserEntry::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (Item.IsValid())
		{
			UAssetEditorSubsystem* AssetEdSub = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
			AssetEdSub->OpenEditorForAsset(Item->AssetData.GetSoftObjectPath());

			return FReply::Handled();
		}
	}
	
	return SCompoundWidget::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
}

FText SPlaysetBrowserEntry::GetNameText() const
{
	FText NameText = FText::GetEmpty();
	
	if (Item.IsValid())
	{
		const UPlayset* Playset = Cast<UPlayset>(Item->AssetData.GetAsset());
		NameText = Playset->DisplayInfo.ItemName;
	}

	if (NameText.IsEmpty())
	{
		NameText = Item->DisplayName;
	}

	return NameText;
}

const FSlateBrush* SPlaysetBrowserEntry::GetBorder() const
{
	if (IsPressed())
	{
		static const FName Selected("ContentBrowser.AssetTileItem.SelectedBorder");
		return FAppStyle::GetBrush(Selected);
	}
	
	if (IsHovered())
	{
		static const FName Hovered("ContentBrowser.AssetTileItem.HoverBorder");
		return FAppStyle::GetBrush(Hovered);
	}

	static const FName Normal("ContentBrowser.AssetTileItem.NameAreaBackground");
	return FAppStyle::GetBrush(Normal);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE