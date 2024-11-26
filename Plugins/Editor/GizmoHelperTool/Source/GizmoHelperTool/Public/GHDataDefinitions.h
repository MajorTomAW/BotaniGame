// Copyright © 2024 Game Shifters Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GHDataDefinitions.generated.h"

#define GH_VIEWPORT "SViewport"
#define GH_CONTENT_BROWSER_LIST "SAssetListView"
#define GH_CONTENT_BROWSER_TILE "SAssetTileView"
#define GH_CONTENT_BROWSER_COLUMN "SAssetColumnView"

/**
 * Collision Selection
 */
UENUM(BlueprintType)
enum class EGizmoCollisionSelection : uint8
{
	/**
	 * Trace for simple collisions
	 */
	SimpleCollision UMETA(DisplayName = "Simple Collisions"),

	/**
	 * Trace for complex collisions
	 */
	ComplexCollision UMETA(DisplayName = "Complex Collisions"),
};

/**
 * Custom bounds for specific actor classes
 */
USTRUCT(BlueprintType)
struct FGHActorCustomBounds
{
	GENERATED_BODY()

	FGHActorCustomBounds()
		: ActorClass(nullptr), CustomBounds(FBoxSphereBounds(FVector::ZeroVector, FVector::ZeroVector, 0.0f))
	{
	}

	FGHActorCustomBounds(TSubclassOf<AActor> InActorClass, const FBoxSphereBounds& InCustomBounds)
		: ActorClass(InActorClass), CustomBounds(InCustomBounds)
	{
	}

	/**
	 * The actor class to which the custom bounds apply
	 */
	UPROPERTY(EditDefaultsOnly, Config, Category = "Bounds")
	TSubclassOf<AActor> ActorClass;

	/**
	 * The custom bounds for the actor class
	 */
	UPROPERTY(EditDefaultsOnly, Config, Category = "Bounds")
	FBoxSphereBounds CustomBounds;
};

/**
 * Stores an actors' relative position (location)
 */
struct FGHActorRelativePosition
{
	FGHActorRelativePosition()
		: Actor(nullptr), RelativePosition(FVector::ZeroVector)
	{
	}
	
	FGHActorRelativePosition(AActor* InActor, const FVector& InRelativePosition)
		: Actor(InActor), RelativePosition(InRelativePosition)
	{
	}
	
	TWeakObjectPtr<AActor> Actor;
	FVector RelativePosition;
};

/**
 * Selection mode
 */
namespace EGizmoSelectionMode
{
	enum Type : uint8
	{
		Viewport,
		ContentBrowser,
		None
	};	
}

/**
 * Bounding Box helper struct
 */
struct FGizmoBoundingBox
{
public:
	FGizmoBoundingBox()
		: Origin(FVector::ZeroVector), Extent(FVector::ZeroVector), Offset(FVector::ZeroVector)
	{
	}

	explicit FGizmoBoundingBox(const AActor* InActor)
	{
		ConstructBoundingBox(InActor);
	}
	
public:
	FVector Origin;
	FVector Extent;
	FVector Offset;

private:
	void ConstructBoundingBox(const AActor* InActor);
};


USTRUCT()
struct FGizmoHelperVars
{
	GENERATED_BODY()

public:
	FGizmoHelperVars() {}
	explicit FGizmoHelperVars(const TArray<AActor*>& InSelectedActors)
	{
		OnConstruct(InSelectedActors);
	}

	~FGizmoHelperVars()
	{
		SelectedActors.Empty();
		AllActors.Empty();
		ChildActors.Empty();
		ActorsToMoveCopy.Empty();
		FirstActor = nullptr;
	}
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> SelectedActors;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> AllActors;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> ActorsToMoveCopy;

	UPROPERTY()
	TObjectPtr<AActor> FirstActor;

	FGizmoBoundingBox BoundingBox;

private:
	TArray<AActor*> ChildActors;

	
	void OnConstruct(const TArray<AActor*>& InSelectedActors);
};