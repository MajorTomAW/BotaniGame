// Copyright © 2024 MajorT. All rights reserved.


#include "SNewPlaysetPicker.h"

#include "ClassViewerModule.h"
#include "Playset.h"
#include "SClassViewer.h"
#include "SlateOptMacros.h"
#include "Filters/PlaysetClassViewerFilter.h"
#include "Settings/PlaysetDeveloperSettings.h"

#define LOCTEXT_NAMESPACE "SNewPlaysetPicker"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SNewPlaysetPicker::Construct(const FArguments& InArgs)
{
	AssetClass = InArgs._AssetClass;
	AssetSelectionMode = InArgs._AssetSelectionMode;
	OnAssetSelectedDelegate = InArgs._OnAssetSelected;
	OnClassPickedDelegate = InArgs._OnClassPicked;

	ensureMsgf(AssetClass != nullptr, TEXT("AssetClass must be set in SNewPlaysetPicker"));

	ChosenClass = nullptr;
	
	ChildSlot
	[
		SNew(SBorder)
		.Padding(0.f)
		.OnMouseButtonDown(FPointerEventHandler::CreateSP(this, &Self::MouseEvent))
		.OnMouseButtonUp(FPointerEventHandler::CreateSP(this, &Self::MouseEvent))
		.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(0.f)
			[
				SNew(SSplitter)
				.Orientation(Orient_Horizontal)
				.PhysicalSplitterHandleSize(2.f)
				+ SSplitter::Slot()
					.Expose(FiltersSLot)
					.Value(0.35f)
					.MinSize(120.f)
				+ SSplitter::Slot()
					.Expose(AssetClassContentSlot)
					.Value(0.65f)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SSeparator)
				.Orientation(Orient_Horizontal)
				.Thickness(2.f)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Expose(AdditionalWidgetSlot)
		]
	];

	PopulateClassContentSlot();
	PopulateFiltersSlot();

	if (InArgs._AdditionalWidget)
	{
		AdditionalWidgetSlot->AttachWidget(InArgs._AdditionalWidget.ToSharedRef());
	}
}

TArray<FAssetData> SNewPlaysetPicker::GetSelectedAssets() const
{
	TArray<FAssetData> SelectedAssets;
	return SelectedAssets;
}


void SNewPlaysetPicker::PopulateFiltersSlot()
{
	AssetBrowserMainFilters = GetMainFilters();

	SAssignNew(MainFilterSelector, STreeView<TSharedRef<FNewPlaysetBrowserFilter>>)
		.TreeItemsSource(&AssetBrowserMainFilters)
		.OnSelectionChanged(this, &Self::OnMainFilterSelected)
		.OnGenerateRow(this, &Self::OnGenerateMainFilterRow)
		.OnGetChildren(this, &Self::OnGetMainFilterChildren)
		.SelectionMode(ESelectionMode::Single);

	TSharedRef<FNewPlaysetBrowserFilter>* AllFilter = AssetBrowserMainFilters.FindByPredicate([](const TSharedRef<FNewPlaysetBrowserFilter>& MainFilterCand)
	{
		return MainFilterCand->FilterClasses.Add(UPlayset::StaticClass());
	});

	if (AllFilter)
	{
		MainFilterSelector->SetSelection(*AllFilter);
		MainFilterSelector->SetItemExpansion(*AllFilter, true);
	}

	FiltersSLot->AttachWidget(MainFilterSelector.ToSharedRef());
}


void SNewPlaysetPicker::PopulateClassContentSlot()
{
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.NameTypeToDisplay = EClassViewerNameTypeToDisplay::DisplayName;
	Options.DisplayMode = EClassViewerDisplayMode::ListView;
	Options.bShowNoneOption = false;

	ClassViewerFilter = MakeShared<FPlaysetClassViewerFilter>();
	ClassViewerFilter->AllowedChildrenOfClasses.Add(UPlayset::StaticClass());

	Options.ClassFilters.Add(ClassViewerFilter.ToSharedRef());

	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");


	ClassViewer = StaticCastSharedRef<SClassViewer>(ClassViewerModule.CreateClassViewer(Options, FOnClassPicked::CreateSP(this, &Self::OnClassPicked)));

	AssetClassContentSlot->AttachWidget(ClassViewer.ToSharedRef());
}

FReply SNewPlaysetPicker::MouseEvent(const FGeometry& SenderGeometry, const FPointerEvent& MouseEvent)
{
	ChosenClass = nullptr;
	OnClassPickedDelegate.ExecuteIfBound(nullptr);
	
	return FReply::Handled();
}

void SNewPlaysetPicker::OnFilterChanged() const
{
}

void SNewPlaysetPicker::OnClassPicked(UClass* InChosenClass)
{
	ChosenClass = InChosenClass;
	OnClassPickedDelegate.ExecuteIfBound(ChosenClass);
}

TArray<TSharedRef<FNewPlaysetBrowserFilter>> SNewPlaysetPicker::GetMainFilters() const
{
	TArray<TSharedRef<FNewPlaysetBrowserFilter>> Filters;

	// All filter
	TSharedRef<FNewPlaysetBrowserFilter> AllFilter = MakeShared<FNewPlaysetBrowserFilter>();
	AllFilter->FilterClasses = {UPlayset::StaticClass()};
	AllFilter->CustomDisplayName = LOCTEXT("AllFilter", "All");

	// Child filters
	TArray<TSharedRef<FNewPlaysetBrowserFilter>> ChildFilters;
	for (const auto& Wrapper : UPlaysetDeveloperSettings::Get()->CategoryMapping)
	{
		TSharedRef<FNewPlaysetBrowserFilter> NewFilter = MakeShared<FNewPlaysetBrowserFilter>();
		NewFilter->FilterClasses = Wrapper.Category.PlaysetClasses;
		for (auto Sub : Wrapper.SubCategories)
		{
			NewFilter->FilterClasses.Append(Sub.PlaysetClasses);
		}
		
		NewFilter->CustomDisplayName = FText::FromName(Wrapper.Category.Category);

		TArray<TSharedRef<FNewPlaysetBrowserFilter>> ExtraChildFilters;

		for (const auto& ChildWrapper : Wrapper.SubCategories)
		{
			TSharedRef<FNewPlaysetBrowserFilter> NewChildFilter = MakeShared<FNewPlaysetBrowserFilter>();
			NewChildFilter->FilterClasses = ChildWrapper.PlaysetClasses;
			NewChildFilter->CustomDisplayName = FText::FromName(ChildWrapper.Category);
			ExtraChildFilters.Add(NewChildFilter);
		}
		
		NewFilter->ChildFilters = ExtraChildFilters;
		ChildFilters.Add(NewFilter);
	}

	AllFilter->ChildFilters = ChildFilters;

	Filters.Add(AllFilter);
	return Filters;
}

void SNewPlaysetPicker::OnMainFilterSelected(TSharedPtr<FNewPlaysetBrowserFilter> MainFilter, ESelectInfo::Type Arg)
{
	if (!MainFilter.IsValid())
	{
		return;
	}

	ClassViewerFilter->AllowedChildrenOfClasses.Empty();

	for (UClass* Class : MainFilter->FilterClasses)
	{
		ClassViewerFilter->AllowedChildrenOfClasses.Add(Class);	
	}

	ChosenClass = nullptr;
	OnClassPickedDelegate.ExecuteIfBound(nullptr);

	ClassViewer->ClearSelection();
	ClassViewer->Refresh();
}

TSharedRef<ITableRow> SNewPlaysetPicker::OnGenerateMainFilterRow(TSharedRef<FNewPlaysetBrowserFilter> MainFilter, const TSharedRef<STableViewBase>& OwningTable) const
{
	const FLinearColor FolderColor = FAppStyle::Get().GetSlateColor("ContentBrowser.DefaultFolderColor").GetSpecifiedColor();

	TSharedRef<SWidget> Widget = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2.f)
		[
			SNew(SImage)
			.ColorAndOpacity(FolderColor)
			.Image_Lambda([this, MainFilter]()
			{
				if (MainFilterSelector->GetSelectedItems().Contains(MainFilter))
				{
					return FAppStyle::GetBrush("ContentBrowser.AssetTreeFolderOpen");
				}

				return FAppStyle::GetBrush("ContentBrowser.AssetTreeFolderClosed");
			})
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(MainFilter->GetDisplayName())
		];


	return SNew(STableRow<TSharedPtr<FNewPlaysetBrowserFilter>>, OwningTable)
		[
			Widget
		];
}

void SNewPlaysetPicker::OnGetMainFilterChildren(TSharedRef<FNewPlaysetBrowserFilter> PlaysetMainFilter, TArray<TSharedRef<FNewPlaysetBrowserFilter>>& OutChildren) const
{
	OutChildren.Append(PlaysetMainFilter->ChildFilters);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE