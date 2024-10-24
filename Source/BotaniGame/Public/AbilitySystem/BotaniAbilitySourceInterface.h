// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BotaniAbilitySourceInterface.generated.h"

class UObject;
class UPhysicalMaterial;
struct FGameplayTagContainer;

/** Base interface for anything acting as a ability calculation source */
UINTERFACE()
class BOTANIGAME_API UBotaniAbilitySourceInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class BOTANIGAME_API IBotaniAbilitySourceInterface
{
	GENERATED_IINTERFACE_BODY()

public:

	/**
	 * Compute the multiplier for effect falloff with distance
	 * 
	 * @param Distance			Distance from source to target for ability calculations (distance a bullet traveled for a gun, etc...)
	 * @param SourceTags		Aggregated Tags from the source
	 * @param TargetTags		Aggregated Tags currently on the target
	 * 
	 * @return Multiplier to apply to the base attribute value due to distance
	 */
	virtual float GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const = 0;

	/**
	 * Compute the multiplier for effect falloff with physical material.
	 * Some physical materials may be more resistant to certain types of effects.
	 * Or a physical material can be used as a weak spot for an entity (headshots, etc...)
	 * 
	 * @param PhysicalMaterial	Physical material of the target for ability calculations
	 * @param SourceTags		Aggregated Tags from the source
	 * @param TargetTags		Aggregated Tags currently on the target
	 * 
	 * @return Multiplier to apply to the base attribute value due to physical material
	 */
	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const = 0;
};
