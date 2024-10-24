// Copyright © 2024 MajorT. All rights reserved.

#include "AssetDefinition_Playset.h"

#include "GamePlaysets/Public/Playset.h"
#include "Style/PlaysetStyle.h"
#include "ThumbnailRendering/SceneThumbnailInfo.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_Playset"

FText UAssetDefinition_Playset::GetAssetDisplayName() const
{
	return LOCTEXT("AssetDisplayName", "Playset Item");
}

FText UAssetDefinition_Playset::GetAssetDisplayName(const FAssetData& AssetData) const
{
	if (UClass* Class = AssetData.GetClass())
	{
		FString ClassName = Class->GetName();

		// Put a space between each word that starts with a capital letter
		for (int32 Index = 1; Index < ClassName.Len(); ++Index)
		{
			if (FChar::IsUpper(ClassName[Index]) && !FChar::IsUpper(ClassName[Index - 1]))
			{
				ClassName.InsertAt(Index, TEXT(" "));
				++Index;
			}
		}

		return FText::Format(LOCTEXT("AssetDisplayName", "{0}"), FText::FromString(ClassName));
	}

	return Super::GetAssetDisplayName();
}

FText UAssetDefinition_Playset::GetAssetDescription(const FAssetData& AssetData) const
{
	const UPlayset* Playset = GetPlaysetFromAssetData(AssetData);
	if (Playset == nullptr)
	{
		return FText();
	}

	return Playset->GetPlaysetDesc();
}

FLinearColor UAssetDefinition_Playset::GetAssetColor() const
{
	return FPlaysetStyle::GetPlaysetColor();
}

TSoftClassPtr<> UAssetDefinition_Playset::GetAssetClass() const
{
	return UPlayset::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_Playset::GetAssetCategories() const
{
	static const auto Categories =
		{
		// Basic category, shown in the content browser
		EAssetCategoryPaths::Basic,

		// Advanced category to show in the advanced section
		FAssetCategoryPath(FText::FromName(FPlaysetEditorIDs::PlaysetAssetCategory()))
		};
	
	return Categories;
}

FAssetSupportResponse UAssetDefinition_Playset::CanRename(const FAssetData& InAsset) const
{
	return FAssetSupportResponse::Supported();
}

FAssetSupportResponse UAssetDefinition_Playset::CanDuplicate(const FAssetData& InAsset) const
{
	return FAssetSupportResponse::Supported();
}

UThumbnailInfo* UAssetDefinition_Playset::LoadThumbnailInfo(const FAssetData& InAssetData) const
{
	UPlayset* Playset = GetPlaysetFromAssetData(InAssetData);
	if (Playset == nullptr)
	{
		return nullptr;
	}

	USceneThumbnailInfo* ThumbnailInfo = UE::Editor::FindOrCreateThumbnailInfo<USceneThumbnailInfo>(Playset);
	return ThumbnailInfo;
}

UPlayset* UAssetDefinition_Playset::GetPlaysetFromAssetData(const FAssetData& AssetData)
{
	UObject* Asset = AssetData.GetAsset();
	if (Asset == nullptr)
	{
		return nullptr;
	}
	
	return Cast<UPlayset>(Asset);
}

#undef LOCTEXT_NAMESPACE