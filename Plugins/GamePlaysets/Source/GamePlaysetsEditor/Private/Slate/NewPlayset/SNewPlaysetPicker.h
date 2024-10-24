// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ClassViewerModule.h"
#include "ContentBrowserDelegates.h"
#include "Filters/NewPlaysetBrowserFilter.h"
#include "Widgets/SCompoundWidget.h"

class FPlaysetClassViewerFilter;
class FFrontendFilter;

class GAMEPLAYSETSEDITOR_API SNewPlaysetPicker : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SNewPlaysetPicker)
		{
		}
		SLATE_ARGUMENT(UClass*, AssetClass)
		SLATE_ARGUMENT(ESelectionMode::Type, AssetSelectionMode)
		SLATE_EVENT(FOnAssetSelected, OnAssetSelected)
		SLATE_EVENT(FOnClassPicked, OnClassPicked)
		SLATE_ARGUMENT(TSharedPtr<SWidget>, AdditionalWidget)
	SLATE_END_ARGS()
	typedef SNewPlaysetPicker Self;

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	TArray<FAssetData> GetSelectedAssets() const;

private:
	void PopulateFiltersSlot();
	void PopulateClassContentSlot();

	FReply MouseEvent(const FGeometry& SenderGeometry, const FPointerEvent& MouseEvent);

private:
	void OnFilterChanged() const;
	void OnClassPicked(UClass* InChosenClass);

	TArray<TSharedRef<FNewPlaysetBrowserFilter>> GetMainFilters() const;

	void OnMainFilterSelected(TSharedPtr<FNewPlaysetBrowserFilter> MainFilter, ESelectInfo::Type Arg);
	TSharedRef<ITableRow> OnGenerateMainFilterRow(TSharedRef<FNewPlaysetBrowserFilter> MainFilter, const TSharedRef<STableViewBase>& OwningTable) const;
	void OnGetMainFilterChildren(TSharedRef<FNewPlaysetBrowserFilter> PlaysetMainFilter, TArray<TSharedRef<FNewPlaysetBrowserFilter>>& OutChildren) const;

private:
	UClass* AssetClass = nullptr;
	FOnClassPicked OnClassPickedDelegate;
	FOnAssetSelected OnAssetSelectedDelegate;
	ESelectionMode::Type AssetSelectionMode = ESelectionMode::Single;

	//TMap<FNewPlaysetBrowserFilter, TSharedRef<FFrontendFilter>> DropdownFilterCache;
	TArray<TSharedRef<FNewPlaysetBrowserFilter>> AssetBrowserMainFilters;

	TSharedPtr<class SClassViewer> ClassViewer;
	TSharedPtr<FPlaysetClassViewerFilter> ClassViewerFilter;
	TSharedPtr<STreeView<TSharedRef<FNewPlaysetBrowserFilter>>> MainFilterSelector;
	
	SSplitter::FSlot* FiltersSLot = nullptr;
	SSplitter::FSlot* AssetClassContentSlot = nullptr;
	SVerticalBox::FSlot* AdditionalWidgetSlot = nullptr;

private:
	UClass* ChosenClass = nullptr;
};
