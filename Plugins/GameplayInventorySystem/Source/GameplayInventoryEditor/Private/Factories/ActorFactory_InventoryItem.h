// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GamePlaysetsEditor/Private/Factories/ActorFactory_Playset.h"
#include "ActorFactory_InventoryItem.generated.h"

UCLASS(Config = Editor, CollapseCategories, HideCategories = Object, MinimalAPI)
class UActorFactory_InventoryItem : public UActorFactory
{
	GENERATED_UCLASS_BODY()

public:
	GAMEPLAYINVENTORYEDITOR_API virtual AActor* GetDefaultActor(const FAssetData& AssetData) override;
	GAMEPLAYINVENTORYEDITOR_API virtual bool CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) override;
	GAMEPLAYINVENTORYEDITOR_API virtual void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
	GAMEPLAYINVENTORYEDITOR_API virtual UObject* GetAssetFromActorInstance(AActor* ActorInstance) override;
	
};
