// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Pickup/PickupProxyActor.h"
#include "UObject/Object.h"
#include "PickupItemIcon.generated.h"

/**
 * UPickupItemIcon
 *
 * A single icon that represents a pickup item in the world.
 * Your pickup can have multiple icons that represent it.
 * Icons can be static meshes, textures, materials, and so on.
 */
UCLASS(MinimalAPI, Blueprintable, BlueprintType, EditInlineNew, Config = Game, Abstract, meta = (DisplayName = "Pickup Icon"))
class UPickupItemIcon : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/**
	 * Called to apply this icon to the specified pickup proxy actor.
	 *
	 * Typically used to spawn a preview mesh or set a material on the pickup.
	 */
	GAMEPLAYINVENTORYSYSTEM_API virtual void ApplyPickupIcon(class APickupProxyActor* InProxy, const class UGameplayInventoryItemDefinition* InItemDef);
	

	/**
	 * Called to apply this icon to the specified pickup proxy actor.
	 *
	 * Typically used to spawn a preview mesh or set a material on the pickup.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup", meta = (DisplayName = "Apply Pickup Icon"))
	GAMEPLAYINVENTORYSYSTEM_API void K2_ApplyPickupIcon(class APickupProxyActor* InProxy, const class UGameplayInventoryItemDefinition* InItemDef);

protected:
#if WITH_EDITORONLY_DATA
	/** The icon's display name to show in the editor. */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "false", EditConditionHides))
	FString EditorFriendlyName;

	/** Gets the editor-friendly name of the icon. */
	UFUNCTION(BlueprintNativeEvent, Category = "Editor")
	FString GetEditorFriendlyName() const;
#endif

#if WITH_EDITOR
	//~ Begin UObject Interface
	virtual void PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext) override;
	virtual void PostCDOCompiled(const FPostCDOCompiledContext& Context) override;
	//~ End UObject Interface
#endif
};


/**
 * UPickupIconModifier
 *
 * A modifier that can be applied to a pickup icon.
 */
UCLASS(MinimalAPI, Blueprintable, BlueprintType, EditInlineNew, Config = Game, Abstract)
class UPickupIconModifier : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Called to apply this modifier to the specified pickup icon. */
	GAMEPLAYINVENTORYSYSTEM_API virtual void ApplyModifier(UPickupItemIcon* Outer, USceneComponent* SpawnedIcon, APickupProxyActor* PickupProxy, const UGameplayInventoryItemDefinition* InItemDef);

	/** Called to apply this modifier to the specified pickup icon. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup", meta = (DisplayName = "Apply Pickup Icon Modifier"))
	GAMEPLAYINVENTORYSYSTEM_API void K2_ApplyModifier(UPickupItemIcon* Outer, USceneComponent* SpawnedIcon, APickupProxyActor* PickupProxy, const UGameplayInventoryItemDefinition* InItemDef);
};
