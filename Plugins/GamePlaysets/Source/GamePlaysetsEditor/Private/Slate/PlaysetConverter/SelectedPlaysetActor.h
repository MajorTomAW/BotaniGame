// Copyright © 2024 MajorT. All rights reserved.
#pragma once

struct FSelectedPlaysetActor
{
public:
	FSelectedPlaysetActor();
	FSelectedPlaysetActor(TWeakObjectPtr<AActor> ActorPtr);
	
	uint8 bEnabled : 1;

	FString GetActorName() const;
	UClass* GetActorClass() const;
	FAssetData GetAssetData() const { return AssetData; }

	bool operator==(const FSelectedPlaysetActor& Other) const;

private:
	FAssetData AssetData;
};