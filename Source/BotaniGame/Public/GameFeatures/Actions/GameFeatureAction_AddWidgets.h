// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameplayTagContainer.h"
#include "UIExtensionSystem.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFeatureAction_AddWidgets.generated.h"

class UCommonActivatableWidget;

////////////////////////////////////////////////////////////////////////
/// FBotaniHUDLayoutRequest
////////////////////////////////////////////////////////////////////////

/**
 * FBotaniHUDLayoutRequest
 */
USTRUCT()
struct FBotaniHUDLayoutRequest
{
	GENERATED_BODY()

public:
	/** The layout widget to spawn */
	UPROPERTY(EditAnywhere, Category = "UI", meta = (AssetBundles = "Client"))
	TSoftClassPtr<UCommonActivatableWidget> LayoutClass;

	/** The layer to insert the widget in */
	UPROPERTY(EditAnywhere, Category = "UI", meta = (Categories = "UI.Layer"))
	FGameplayTag LayerID;
};

////////////////////////////////////////////////////////////////////////
/// FBotaniHUDElementEntry
////////////////////////////////////////////////////////////////////////

/**
 * FBotaniHUDElementEntry
 */
USTRUCT()
struct FBotaniHUDElementEntry
{
	GENERATED_BODY()

public:
	/** The widget to spawn */
	UPROPERTY(EditAnywhere, Category = "UI", meta = (AssetBundles = "Client"))
	TSoftClassPtr<UUserWidget> WidgetClass;

	/** The slot ID where we should place this widget */
	UPROPERTY(EditAnywhere, Category = "UI", meta = (Categories = "HUD.Slot"))
	FGameplayTag SlotID;
};

////////////////////////////////////////////////////////////////////////
/// UGameFeatureAction_AddWidgets
////////////////////////////////////////////////////////////////////////

/**
 * UGameFeatureAction_AddWidgets
 *
 * Game Feature Action responsible for adding widgets to a specific layer to the HUD or Menu.
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Widgets"))
class UGameFeatureAction_AddWidgets final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()
public:
	//~ Begin UGameFeatureAction interface
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
#if WITH_EDITORONLY_DATA
	virtual void AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData) override;
#endif
	//~ End UGameFeatureAction interface

	//~ Begin UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject interface

protected:
	/** List of layouts to add to the HUD */
	UPROPERTY(EditAnywhere, Category = "UI", meta = (TitleProperty = "{LayerID} -> {LayoutClass}"))
	TArray<FBotaniHUDLayoutRequest> Layouts;

	/** List of widget extensions to add to the HUD extension points */
	UPROPERTY(EditAnywhere, Category = "UI", meta = (TitleProperty = "{SlotID} -> {WidgetClass}"))
	TArray<FBotaniHUDElementEntry> WidgetExtensions;

private:
	struct FPerActorData
	{
		TArray<TWeakObjectPtr<UCommonActivatableWidget>> LayoutsAdded;
		TArray<FUIExtensionHandle> ExtensionHandles;
	};

	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;
		TMap<FObjectKey, FPerActorData> ActorData;
	};
	typedef TMap<FGameFeatureStateChangeContext, FPerContextData> FContextDataMap;
	FContextDataMap ContextData;

	//~ Begin UGameFeatureAction_WorldActionBase interface
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
	//~ End UGameFeatureAction_WorldActionBase interface

	void Reset(FPerContextData& ActiveData);
	void HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
	void AddWidgets(AActor* Actor, FPerContextData& ActiveData);
	void RemoveWidgets(AActor* Actor, FPerContextData& ActiveData);
};
