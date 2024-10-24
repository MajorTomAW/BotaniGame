// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Playset.h"
#include "BotaniCharacterDefinition.generated.h"

/**
 * FBotaniMontageLookupTable
 *
 * Lookup table that maps gameplay tags to montages.
 */
USTRUCT(BlueprintType)
struct FBotaniMontageLookupTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	/** The gameplay tag of the montage. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	FGameplayTagContainer GameplayTags;

	/** The montage to play. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	TSoftObjectPtr<UAnimMontage> AnimMontage;
};

/**
 * EBotaniCharacterCosmeticRelevancy
 *
 * Describes the relevancy of a cosmetic for a character.
 */
UENUM(BlueprintType)
enum class EBotaniCharacterCosmeticRelevancy : uint8
{
	/** The cosmetic is only relevant for third person. */
	ThirdPerson,

	/** The cosmetic is only relevant for first-person. */
	FirstPerson,

	/** The cosmetic is relevant for both third and first person. */
	Both
};

/**
 * FBotaniCharacterAccessories
 *
 * Single accessory for a character.
 */
USTRUCT(BlueprintType)
struct FBotaniCharacterAccessories
{
	GENERATED_BODY()

public:
	/** The mesh of the accessory. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessories")
	TSoftObjectPtr<USkeletalMesh> Mesh;

	/** The relevancy of the accessory. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessories")
	EBotaniCharacterCosmeticRelevancy Relevancy = EBotaniCharacterCosmeticRelevancy::Both;

	/** The animation blueprint of the accessory. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessories")
	TSoftClassPtr<UAnimInstance> AnimationBlueprint;

	/** The attach socket of the accessory. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessories")
	FName AttachSocket = NAME_None;

	/** The relative transform of the accessory. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessories")
	FTransform RelativeTransform;
};

/**
 * UBotaniCharacterDefinition
 *
 * Stores cosmetic data for a character.
 * Such as the character's mesh, material, and color.
 */
UCLASS(meta = (DisplayName = "Botani Character Definition"))
class UBotaniCharacterDefinition : public UPlayset
{
	GENERATED_UCLASS_BODY()

public:
	/** The relevancy of this cosmetic. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cosmetic")
	EBotaniCharacterCosmeticRelevancy Relevancy = EBotaniCharacterCosmeticRelevancy::Both;
	
	/** The base skeletal mesh of the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cosmetic|Base", meta = (EditCondition = "Relevancy != EBotaniCharacterCosmeticRelevancy::FirstPerson", EditConditionHides))
	TSoftObjectPtr<USkeletalMesh> BaseMesh;

	/** The relative transform of the base mesh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cosmetic|Base", meta = (EditCondition = "Relevancy != EBotaniCharacterCosmeticRelevancy::FirstPerson", EditConditionHides))
	FTransform BaseRelativeTransform;

	/** The first-person skeletal mesh of the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cosmetic|FirstPerson", meta = (EditCondition = "Relevancy != EBotaniCharacterCosmeticRelevancy::ThirdPerson", EditConditionHides))
	TSoftObjectPtr<USkeletalMesh> FirstPersonMesh;

	/** The relative transform of the first-person mesh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cosmetic|FirstPerson", meta = (EditCondition = "Relevancy != EBotaniCharacterCosmeticRelevancy::ThirdPerson", EditConditionHides))
	FTransform FirstPersonRelativeTransform;

	/** The accessories of the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessories")
	TArray<FBotaniCharacterAccessories> Accessories;

	/** The montages of the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages", meta = (RequiredAssetDataTags = "RowStructure=/Script/BotaniGame.BotaniMontageLookupTable"))
	TObjectPtr<UDataTable> MontageLookupTable;
};
