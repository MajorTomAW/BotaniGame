// Copyright © 2024 MajorT. All rights reserved.

#include "PlaysetTypes.h"

#include "GameplayTags/PlaysetGameplayTags.h"

FPlaysetDisplayInfo::FPlaysetDisplayInfo()
{
}

FPlaysetDataList::FPlaysetDataList()
	: MinLevel(0)
	, MaxLevel(0)
{
	Traits.AddTag(PlaysetGameplayTags::Traits::TAG_Trait_Transient);
	Traits.AddTag(PlaysetGameplayTags::Traits::TAG_Trait_SingleStack);
}

UTexture2D* FPlaysetDataList::GetThumbnailImage() const
{
	UTexture2D* ThumbnailImage = nullptr;
	
	if (!Icon.IsNull())
	{
		ThumbnailImage = Icon.Get();

		if (ThumbnailImage == nullptr)
		{
			ThumbnailImage = Icon.LoadSynchronous();
		}
	}

	if (!LargeIcon.IsNull() && ThumbnailImage == nullptr)
	{
		ThumbnailImage = LargeIcon.Get();

		if (ThumbnailImage == nullptr)
		{
			ThumbnailImage = LargeIcon.LoadSynchronous();
		}
	}

	return ThumbnailImage;
}

FPlaysetActorData::FPlaysetActorData()
{
	InfluenceDistance = 0.0f;
	ActorTemplateID = INDEX_NONE;
	RelativeLocation = FVector::ZeroVector;
	RelativeRotation = FRotator::ZeroRotator;
	RelativeRotationQuat = FRotator::ZeroRotator;
	RelativeScale = FVector::OneVector;
}

bool FPlaysetActorData::operator==(const FPlaysetActorData& Other) const
{
	return (Other.ActorClass == ActorClass &&
		Other.RelativeLocation == RelativeLocation &&
		Other.RelativeRotation == RelativeRotation &&
		Other.RelativeScale == RelativeScale &&
		Other.InfluenceDistance == InfluenceDistance);
}

#if WITH_EDITORONLY_DATA
void FPlaysetActorData::CollectActorName()
{
	if (ActorClass)
	{
		FString ClassName = ActorClass.GetAssetName();

		// Remove the "_C" suffix
		if (ClassName.EndsWith(TEXT("_C")))
		{
			ClassName.RemoveAt(ClassName.Len() - 2, 2);
		}
		
		ActorName = FName(ClassName);
	}
	else
	{
		ActorName = "None";
	}
}
#endif
