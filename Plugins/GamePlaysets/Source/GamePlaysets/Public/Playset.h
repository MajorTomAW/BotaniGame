// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PlaysetTypes.h"
#include "Engine/DataAsset.h"

#include "Playset.generated.h"

/**
 * @class UPlayset
 *
 * Adds a system for creating and managing playsets, which are collections of actors that can be used to define gameplay rules, objectives, and more.
 */
UCLASS(BlueprintType, PrioritizeCategories = ("Playset") , Blueprintable, meta = (ShortToolTip = "A collection of actors that can be used to define gameplay rules, objectives, and more."))
class GAMEPLAYSETS_API UPlayset : public UPrimaryDataAsset
{
	GENERATED_UCLASS_BODY()

	UPlayset(const FPlaysetDisplayInfo& InDisplayInfo, const FPlaysetDataList& InDataList);

public:
	/** Converts the description text to a multi-line text and returns it. */
	virtual FText GetPlaysetDesc() const;
	/** Converts the description text to a multi-line text and returns it. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Playset", meta = (DisplayName = "Get Item Description"))
	FText K2_GetItemDescription() const { return GetPlaysetDesc(); }

	/** Gets the location offset of the playset (if applicable). */
	virtual FVector GetPlaysetLocationOffset() const;

	/** Returns the playset tags. */ 
	virtual const FGameplayTagContainer& GetGameplayTags() const { return PlaysetTags; }

public:
#if WITH_EDITOR
	virtual void InitializeDisplayInfo(const FPlaysetDisplayInfo& InDisplayInfo);
	virtual void InitializeDataList(const FPlaysetDataList& InDataList);
	virtual void InitializeOffsetType(EPlaysetOffsetType InOffsetType);
	virtual void InitializeSavedActors(TArray<FAssetData> InActorData);
	
	virtual FName GetDefaultSubCategoryID() const {return DefaultSubCategoryID; };

	uint32 bHasCustomActorFactory : 1 = 0;
#endif

protected:
#if WITH_EDITOR
	//~ Begin UObject Interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext) override;
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	//~ End UObject Interface

	virtual void InitializeSavedActor_Internal(const FVector& RelativeOrigin, const FAssetData& InActorData);
#endif

public:
	/** Returns the playset tags. */
	UFUNCTION(BlueprintCallable, Category = "Playset", meta = (DisplayName = "Get Const Gameplay Tags"))
	const FGameplayTagContainer& K2_GetGameplayTags() const { return GetGameplayTags(); }

	/** Returns the display name of the playset. */
	UFUNCTION(BlueprintCallable, Category = "Playset")
	FText GetDisplayName() const { return DisplayInfo.ItemName; }

	/** Returns the description of the playset. */
	UFUNCTION(BlueprintCallable, Category = "Playset")
	FText GetItemShortDescription() const { return DisplayInfo.ItemShortDescription;}

	/** Returns the display name as a string. */
	UFUNCTION(BlueprintCallable, Category = "Playset")
	FString GetDisplayNameString() const { return DisplayInfo.ItemName.ToString(); }

	/** Tries to return the icon of the playset. */
	UFUNCTION(BlueprintCallable, Category = "Playset")
	UTexture2D* GetIcon(const bool bUseLargeIfNotFound = true) const;
	

public:
	/******************************* General *******************************/

	/** The display info of the playset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset")
 	FPlaysetDisplayInfo DisplayInfo;
	
	/** The data list of the playset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset")
	FPlaysetDataList DataList;

	/** The documentation url of the playset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset", AdvancedDisplay, meta = (DisplayName = "Documentation URL"))
	FString DocumentationURL;

#if WITH_EDITORONLY_DATA
	/** The default sub-category of the playset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset", AdvancedDisplay, meta = (DisplayName = "Default Sub Category"))
	FName DefaultSubCategoryID;
#endif

	/******************************* Tags *******************************/

	/** The tags of the playset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset|Tags")
	FGameplayTagContainer PlaysetTags;

	/******************************* Placement *******************************/
	
	/** Whether the playset should adjust for world collision. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset|Placement")
	uint8 bAdjustForWorldCollision : 1;

	/** The size of this playset on the Z axis. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset|Placement|Virtual Extend", meta = (EditCondition = "bAdjustForWorldCollision", Units = m, ClampMin = 0, UIMin = 0))
	int32 SizeZ;

	/** The size of this playset on the X axis. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset|Placement|Virtual Extend", meta = (EditCondition = "bAdjustForWorldCollision", Units = m, ClampMin = 0, UIMin = 0))
	int32 SizeX;

	/** The size of this playset on the Y axis. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset|Placement|Virtual Extend", meta = (EditCondition = "bAdjustForWorldCollision", Units = m, ClampMin = 0, UIMin = 0))
	int32 SizeY;
	
	/** Whether the playset should use location offset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset|Placement")
	uint8 bUseLocationOffset : 1;
	
	/** The type of how the playset is offset from the origin. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Playset|Placement", meta = (EditCondition = "bUseLocationOffset"))
	EPlaysetOffsetType OffsetType;

	/** The location offset of the playset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset|Placement", meta = (EditCondition = "bUseLocationOffset"))
	FVector LocationOffset;

	/** The default rotation of the playset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset|Placement")
	FRotator DefaultRotation;

	/******************************* Spawning *******************************/

	/** The number of actors to spawn per class. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset|Spawning")
	TMap<TSoftClassPtr<AActor>, UPARAM(meta = (ClampMin = 0, UIMin = 0)) int32> ActorClassCount;
	
	/** The actor data of this playset. Defines the actors that will be spawned. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Playset|Spawning", meta = (TitleProperty = "{ActorName} -> {ActorTemplateID}"))
	TArray<FPlaysetActorData> ActorData;

	/******************************* Records *******************************/
	//@TODO: Records
};