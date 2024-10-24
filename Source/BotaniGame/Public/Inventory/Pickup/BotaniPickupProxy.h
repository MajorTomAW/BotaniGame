// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Pickup/PickupProxyActor.h"
#include "BotaniPickupProxy.generated.h"

/**
 * EBotaniInitialItemMovement
 *
 * Initial movement of item spawned.
 * @note NONE: Item is not tossed or falls on spawn.
 * @note GRAVITY: Item falls on spawn.
 * @note TOSS: Item is tossed on spawn.
 */
UENUM(BlueprintType)
enum class EBotaniInitialItemMovement : uint8
{
	/** Pickup doesn't move. */
	None,

	/** Pickup falls on spawn. */
	Gravity,

	/** Toss the pickup on spawn. */
	Toss
};

/**
 * ABotaniPickupProxy
 *
 * An pickup proxy actor for Botani pickups.
 */
UCLASS(Abstract)
class BOTANIGAME_API ABotaniPickupProxy : public APickupProxyActor
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	//~ Begin APickupProxyActor Interface
	virtual void OnDropped(AActor* InInstigator, UGameplayInventoryItemDefinition* InItemDefinition, const FInventoryItemPickupData& InPickupData) override;
	//~ End APickupProxyActor Interface

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

protected:
	//~ Begin APickupProxyActor Interface
	virtual void OnPickupCollisionBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	//~ End APickupProxyActor Interface
	
protected:
	/** The light component used to illuminate the pickup. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UPointLightComponent> PickupLight;

	/** The niagara component used to emit particles for the pickup. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UNiagaraComponent> PickupGlowSystem;
	
	/** The movement component used to move the pickup. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UBotaniProjectileMovementComponent> PickupMovement;

	/**
	 * Initial movement of the item spawned.
	 * @note NONE: Item is not tossed or falls on spawn.
	 * @note GRAVITY: Item falls on spawn.
	 * @note TOSS: Item is tossed on spawn.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	EBotaniInitialItemMovement InitialMovement;

	/**
	 * Initial pickup interaction delay.
	 * Makes sure the pickup doesn't get picked up immediately after spawning/dropping.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	float InitialInteractionDelay;
	
private:
	void DisablePickupMovement();
	void EnablePickupMovement_Gravity();
};
