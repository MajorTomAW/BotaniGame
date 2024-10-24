// Copyright © 2024 MajorT. All rights reserved.

#include "SelectedPlaysetActor.h"

FSelectedPlaysetActor::FSelectedPlaysetActor()
{
	bEnabled = true;
}

FSelectedPlaysetActor::FSelectedPlaysetActor(TWeakObjectPtr<AActor> ActorPtr)
{
	bEnabled = true;
	AssetData = ActorPtr.Get(false);
}

FString FSelectedPlaysetActor::GetActorName() const
{
	const AActor* Actor = CastChecked<AActor>(AssetData.GetAsset());
	return Actor->GetActorLabel();
}

UClass* FSelectedPlaysetActor::GetActorClass() const
{
	return Cast<AActor>(AssetData.GetAsset())->GetClass();
}

bool FSelectedPlaysetActor::operator==(const FSelectedPlaysetActor& Other) const
{
	return AssetData == Other.AssetData;
}
