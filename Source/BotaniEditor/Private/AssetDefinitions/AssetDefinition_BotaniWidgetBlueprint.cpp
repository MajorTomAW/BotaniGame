// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AssetDefinition_BotaniWidgetBlueprint.h"

#include "SBlueprintDiff.h"
#include "WidgetBlueprintEditor.h"
#include "Widgets/BotaniUserWidget.h"

#define LOCTEXT_NAMESPACE "AssetDefinition_BotaniWidgetBlueprint"

UAssetDefinition_BotaniWidgetBlueprint::UAssetDefinition_BotaniWidgetBlueprint()
{
}

FText UAssetDefinition_BotaniWidgetBlueprint::GetAssetDisplayName() const
{
	return LOCTEXT("AssetTypeActions_WidgetBlueprint", "Botani Widget Blueprint");
}

FLinearColor UAssetDefinition_BotaniWidgetBlueprint::GetAssetColor() const
{
	return FLinearColor(FColor(44, 89, 180));
}

TSoftClassPtr<UObject> UAssetDefinition_BotaniWidgetBlueprint::GetAssetClass() const
{
	return UBotaniUserWidget::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_BotaniWidgetBlueprint::GetAssetCategories() const
{
	static const TArray<FAssetCategoryPath, TFixedAllocator<1>> Categories = {EAssetCategoryPaths::Basic };
	return Categories;
}

EAssetCommandResult UAssetDefinition_BotaniWidgetBlueprint::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	EToolkitMode::Type Mode = OpenArgs.GetToolkitMode();

	EAssetCommandResult Result = EAssetCommandResult::Unhandled;

	for (UBlueprint* Blueprint : OpenArgs.LoadObjects<UBlueprint>())
	{
		if (Blueprint && Blueprint->SkeletonGeneratedClass && Blueprint->GeneratedClass)
		{
			TSharedRef<FWidgetBlueprintEditor> NewBlueprintEditor(new FWidgetBlueprintEditor);

			const bool bShouldOpenInDefaultsMode = false;
			TArray<UBlueprint*> Blueprints;
			Blueprints.Add(Blueprint);

			NewBlueprintEditor->InitWidgetBlueprintEditor(Mode, OpenArgs.ToolkitHost, Blueprints, bShouldOpenInDefaultsMode);
		}
		else
		{
			FMessageDialog::Open( EAppMsgType::Ok, LOCTEXT("FailedToLoadWidgetBlueprint", "Widget Blueprint could not be loaded because it derives from an invalid class.\nCheck to make sure the parent class for this blueprint hasn't been removed!"));
		}

		Result = EAssetCommandResult::Handled;
	}

	return Result;
}

EAssetCommandResult UAssetDefinition_BotaniWidgetBlueprint::PerformAssetDiff(const FAssetDiffArgs& DiffArgs) const
{
	const UBlueprint* OldBlueprint = Cast<UBlueprint>(DiffArgs.OldAsset);
	const UBlueprint* NewBlueprint = Cast<UBlueprint>(DiffArgs.NewAsset);
	UClass* AssetClass = GetAssetClass().Get();
	SBlueprintDiff::CreateDiffWindow(OldBlueprint, NewBlueprint, DiffArgs.OldRevision, DiffArgs.NewRevision, AssetClass);
	return EAssetCommandResult::Handled;
}

FText UAssetDefinition_BotaniWidgetBlueprint::GetAssetDescription(const FAssetData& AssetData) const
{
	FString Description = AssetData.GetTagValueRef<FString>( GET_MEMBER_NAME_CHECKED( UBlueprint, BlueprintDescription ) );
	if ( !Description.IsEmpty() )
	{
		Description.ReplaceInline( TEXT( "\\n" ), TEXT( "\n" ) );
		return FText::FromString( MoveTemp(Description) );
	}

	return FText::GetEmpty();
}

#undef LOCTEXT_NAMESPACE