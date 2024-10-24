// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AssetDefinition_AIDecisionRule.h"

#include "AIDecisionRule.h"
#include "CommonAIEditorModule.h"
#include "Factories/BlueprintFactory.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText UAssetDefinition_AIDecisionRule::GetAssetDisplayName() const
{
	return LOCTEXT("AssetTypeActions_AIDecisionRule", "AI Decision Rule");
}

FLinearColor UAssetDefinition_AIDecisionRule::GetAssetColor() const
{
	return FLinearColor(FColor( 63, 126, 255 ));
}

TSoftClassPtr<UObject> UAssetDefinition_AIDecisionRule::GetAssetClass() const
{
	return UAIDecisionRule::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_AIDecisionRule::GetAssetCategories() const
{
	static const FAssetCategoryPath Categories[] = { EAssetCategoryPaths::AI };
	return Categories;
}

/*EAssetCommandResult UAssetDefinition_AIDecisionRule::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	const EToolkitMode::Type Mode = OpenArgs.GetToolkitMode();


	for (UAIDecisionRule* DecisionRule : OpenArgs.LoadObjects<UAIDecisionRule>())
	{
		ICommonAIEditorModule& CommonAIEditorModule = ICommonAIEditorModule::Get();
		CommonAIEditorModule.CreateAIDecisionEditorApp(Mode, OpenArgs.ToolkitHost, DecisionRule);
	}

	return EAssetCommandResult::Handled;
}*/

#undef LOCTEXT_NAMESPACE