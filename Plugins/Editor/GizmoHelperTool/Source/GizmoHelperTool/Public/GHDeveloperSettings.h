// Copyright © 2024 Game Shifters Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GHDeveloperSettings.generated.h"

enum class EGizmoCollisionSelection : uint8;
struct FGHActorCustomBounds;

/**
 * Custom settings for the Gizmo Helper Tool
 */
UCLASS(Config = EditorPerProjectUserSettings, DisplayName = "Gizmo Helper Tool", meta = (DisplayName = "Gizmo Helper Tool"))
class GIZMOHELPERTOOL_API UGHDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UGHDeveloperSettings();

	// Static function to get the singleton settings object
	static UGHDeveloperSettings* Get();

	// Static function to get all input keys
	static TArray<FKey> GetAllInputKeys();

	//~ Begin UDeveloperSettings Interface
	virtual FName GetCategoryName() const override;
	//~ End UDeveloperSettings Interface

	/**
	 * Whether to share settings between all projects
	 */
	/* UPROPERTY(EditDefaultsOnly, Config, Category = "Gizmo Helper Tool")*/
	uint8 bGlobalSettings:1 = true;
	
	/**
	 * Key to move the selected actor to the mouse cursor, stopping at the first collision
	 */
	UPROPERTY(EditAnywhere, Config, Category = "Gizmo Helper Tool|Keybinds", meta = (DisplayName = "Move to Mouse Cursor (Respecting Collisions)"))
	FKey MoveToMouseKey;

	/**
	 * Key to move the selected actor to the mouse cursor, without respecting any collisions
	 */
	UPROPERTY(EditAnywhere, Config, Category = "Gizmo Helper Tool|Keybinds", meta = (DisplayName = "Move to Mouse Cursor (Not Respecting Collisions)"))
	FKey MoveToMouseKeyWithoutCollision;

	/**
	 * Control whether to trace for simple or complex collisions when moving an actor.
	 * Complex collision is more accurate.
	 * Simple collision may be inaccurate but less performance demanding.
	 */
	UPROPERTY(EditAnywhere, Config, Category = "Gizmo Helper Tool|Collision", meta = (DisplayName = "Collision Selection"))
	EGizmoCollisionSelection CollisionSelection;

	/**
	 * When trying to place an actor in the world, if no hit is found,
	 * (for example, the mouse cursor is not hovering a surface),
	 * the actor will be placed at the camera location.
	 * When turned off, no actor will be placed if no hit is found.
	 */
	UPROPERTY(EditDefaultsOnly, Config, Category = "Gizmo Helper Tool|Content Browser", meta = (DisplayName = "Place Actors at Camera if no Hit is detected"))
	uint8 bPlaceActorsAtCameraIfNoHit : 1;
	
	/**
	 * Custom bounds scale for specific actor classes
	 * Can be used for objects that don't have a bounding box that fits the object well
	 */
	UPROPERTY(EditAnywhere, Config, Category = "Gizmo Helper Tool|Bounds")
	TArray<FGHActorCustomBounds> CustomBounds;

	/**
	 * Fallback custom bounds for actors that don't have a custom bounds set
	 *
	 * COMING SOON
	 */
	/*UPROPERTY(EditDefaultsOnly, Config, Category = "Gizmo Helper Tool|Bounds")*/
	FBoxSphereBounds FallbackCustomBounds; //TODO ask @Vadim to add a new issue in github as an enhancement to make the origin of the bounds working too

public:
	TArray<TSubclassOf<AActor>> InternalVirtualActors;

	bool TraceToComplexCollision() const;

private:
	void SetDefaultValues();
	void SetKeybindings();
};