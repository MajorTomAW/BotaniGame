// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Actor.h"
#include "BotaniTaggedActor.generated.h"

/**
 * ABotaniTaggedActor
 *
 * An actor that supports gameplay tags without requiring an ability system component.
 */
UCLASS()
class BOTANIGAME_API ABotaniTaggedActor : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin IGameplayTagAssetInterface Interface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	//~ End IGameplayTagAssetInterface Interface

	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif
	//~ End UObject Interface
	
protected:
	/** Gameplay-related tags associated with this actor */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	FGameplayTagContainer StaticGameplayTags;
};
