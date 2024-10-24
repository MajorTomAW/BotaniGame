// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PhysicalMaterialWithTags.generated.h"

/**
 * UPhysicalMaterialWithTags
 *
 * A physical material with gameplay tags that game code can use to reason about this physical material
 */
UCLASS(MinimalAPI)
class UPhysicalMaterialWithTags : public UPhysicalMaterial
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns the gameplay tags associated with this physical material */
	BOTANIGAME_API FGameplayTagContainer GetTags() const { return Tags; }

protected:
	/** A container of gameplay tags that game code can use to reason about this physical material. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=PhysicalProperties)
	FGameplayTagContainer Tags;
};
