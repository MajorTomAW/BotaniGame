// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BotaniExplosionTemplate.generated.h"

/**
 * ABotaniExplosionTemplate
 *
 * A spawnable effect template for explosions in BotaniGame.
 * Each explosion type should be defined as a separate template.
 * 
 * @note: NOT replicated to clients.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, meta = (ShortTooltip = "A spawnable effect template for explosions in BotaniGame."))
class BOTANIGAME_API ABotaniExplosionTemplate : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	/** The explosion effect. */
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> ExplosionEffect;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
private:
};
