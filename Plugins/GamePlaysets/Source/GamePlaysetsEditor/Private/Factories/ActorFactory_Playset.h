// Copyright © 2024 MajorT. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "ActorFactories/ActorFactory.h"
#include "ActorFactory_Playset.generated.h"


UCLASS(Config = Editor, CollapseCategories, HideCategories = Object, MinimalAPI)
class UActorFactory_Playset : public UActorFactory
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UActorFactory Interface
	GAMEPLAYSETSEDITOR_API virtual void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
	GAMEPLAYSETSEDITOR_API virtual void EndPlacement(TArrayView<const FTypedElementHandle> InPlacedElements, const FPlacementOptions& InPlacementOptions) override;
	GAMEPLAYSETSEDITOR_API virtual void PostPlaceAsset(TArrayView<const FTypedElementHandle> InHandle, const FAssetPlacementInfo& InPlacementInfo, const FPlacementOptions& InPlacementOptions) override;
	
	GAMEPLAYSETSEDITOR_API virtual AActor* GetDefaultActor(const FAssetData& AssetData) override;
	GAMEPLAYSETSEDITOR_API virtual bool CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) override;
	GAMEPLAYSETSEDITOR_API virtual UObject* GetAssetFromActorInstance(AActor* ActorInstance) override;
	//~ End UActorFactory Interface

protected:
	GAMEPLAYSETSEDITOR_API virtual void HandlePlacement_Internal(TArrayView<const FTypedElementHandle> InPlacedElements, const FPlacementOptions& InPlacementOptions);
	GAMEPLAYSETSEDITOR_API virtual void HandlePreviewPlacement_Internal(TArrayView<const FTypedElementHandle> InPlacedElements, const FPlacementOptions& InPlacementOptions);
	
	GAMEPLAYSETSEDITOR_API virtual void SpawnPlaysetActors(class AActor* RootActor, const class UPlayset* Playset, const ULevel* InLevel);
	GAMEPLAYSETSEDITOR_API virtual void SpawnPlaysetPreviewActors(class AActor* RootActor, const class UPlayset* Playset, const ULevel* InLevel);

	GAMEPLAYSETSEDITOR_API static AActor* GetActorFromHandle(TArrayView<const FTypedElementHandle> InHandle);

	GAMEPLAYSETSEDITOR_API virtual bool HasPlaysetJustOneActor(const UPlayset* Playset) const;

private:
	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedPreviewActors;
};
