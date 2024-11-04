// Copyright © 2024 MajorT. All rights reserved.

#pragma once
#include "GameplayTagContainer.h"

#include "PlaysetTypes.generated.h"

/**
 * EPlaysetOffsetType
 *
 * The type of how the playset is offset from the origin
 */
UENUM(BlueprintType)
enum class EPlaysetOffsetType : uint8
{
	// The playset has its origin at the center.
	Center,

	// The playset has its origin at the front.
	FrontLet,

	// The playset has its origin at the back.
	FrontRight,

	// The playset has its origin at the left.
	BackLeft,

	// The playset has its origin at the right.
	BackRight,
};

/**
 * FPlaysetDisplayInfo
 *
 * Contains the display information of a playset.
 */
USTRUCT(BlueprintType)
struct GAMEPLAYSETS_API FPlaysetDisplayInfo
{
	GENERATED_BODY()
	FPlaysetDisplayInfo();
	
public:
	/** The name of the playset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset")
	FText ItemName;

	/** The description of the playset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset")
	FText ItemDescription;

	/** The short description of the playset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset")
	FText ItemShortDescription;
};

/**
 * FPlaysetDataList
 *
 * Contains a list of playset data.
 */
USTRUCT(BlueprintType)
struct GAMEPLAYSETS_API FPlaysetDataList
{
	GENERATED_BODY()

	FPlaysetDataList();

public:
	/** Gets the thumbnail image of the playset. Performs a blocking load if required. */
	UTexture2D* GetThumbnailImage() const;

public:
	/** The minimum level of the playset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset", meta = (ClampMin = "0", UIMin = "0"))
	int32 MinLevel;

	/** The maximum level of the playset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset", meta = (ClampMin = "0", UIMin = "0"))
	int32 MaxLevel;

	/** The icon of the playset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset")
	TObjectPtr<UTexture2D> Icon;

	/** The large icon of the playset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset")
	TSoftObjectPtr<UTexture2D> LargeIcon;

	/** The traits of the playset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset", meta = (Categories = "Item.Trait"))
	FGameplayTagContainer Traits;
};

/**
 * FPlaysetActorData
 *
 * The actor data of a playset used when spawning actors.
 */
USTRUCT(BlueprintType)
struct GAMEPLAYSETS_API FPlaysetActorData
{
	GENERATED_BODY()
	FPlaysetActorData();

	bool operator==(const FPlaysetActorData& Other) const;
	
public:
	/** The actor class. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Playset")
	TSoftClassPtr<AActor> ActorClass;

	/** The relative location of the actor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Playset")
	FVector RelativeLocation;

	/** The relative rotation of the actor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Playset", meta = (DisplayName = "Relative Rotation (Euler)"))
	FRotator RelativeRotation;

	/** The relative rotation of the actor as a quaternion. */
	UPROPERTY(VisibleDefaultsOnly, Category = "Playset", meta = (DisplayName = "Relative Rotation (Quaternion)"))
	FRotator /*FQuat*/ RelativeRotationQuat;

	/** The relative scale of the actor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Playset")
	FVector RelativeScale;

	/** A list of additional actor components. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset")
	TArray<TSoftClassPtr<UActorComponent>> ActorComponents;

	/** The influence-distance of the actor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Playset", meta = (ClampMin = "0.0", UIMin = "0.0", Units = cm))
	float InfluenceDistance;

	/** The actor template id. Negative value means this data will be applied to all actor classes. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Playset", meta = (DisplayName = "Actor Template ID"))
	int32 ActorTemplateID;

public:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, meta = (EditCondition = "false", EditConditionHides))
	FName ActorName;

	void CollectActorName();
#endif
};
