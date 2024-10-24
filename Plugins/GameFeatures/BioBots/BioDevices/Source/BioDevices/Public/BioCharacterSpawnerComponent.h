// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Game/Components/BotaniGameplayReceiverMessageComponent.h"
#include "BioCharacterSpawnerComponent.generated.h"

/**
 * UBioCharacterSpawnerComponent
 *
 * An actor component that manages spawning character AI's.
 */
UCLASS(meta = (BlueprintSpawnableComponent), HideCategories = (Activation))
class BIODEVICES_API UBioCharacterSpawnerComponent : public UBotaniGameplayReceiverMessageComponent
{
	GENERATED_UCLASS_BODY()

	/** Delegate used to determine the spawn location for the next AI */
	DECLARE_DELEGATE_RetVal(FVector, FGetSpawnLocationDelegate);

public:
	/** Delegate used to determine the spawn location for the next AI */
	FGetSpawnLocationDelegate OnGetSpawnLocation;

	/** Starts the spawning process */
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void StartSpawning();

	/** Stops the spawning process */
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void StopSpawning();
	
protected:
	/**
	 * Sets the number of AIs this spawner can have active at any time.
	 * When the spawner activates, it will produce one AI at a time.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner", meta = (ClampMin = 1, UIMin = 1, ClampMax = 20, UIMax = 20))
	int32 SpawnCount;

	/** The Bot Definition that will be used to spawn the AI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	TObjectPtr<class UBotaniBotDefinition> BotDefinition;

		
	/** When the device is disabled, this determines whether already spawned AIs remain or are despawned. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", AdvancedDisplay, meta = (DisplayName = "Despawn AIs when Disabled"))
	uint32 bDespawnAIOnDisable : 1;

protected:
	//~ Begin UActorComponent Interface
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	//~ End UActorComponent Interface
	
	/** Get the location to spawn the AI */
	virtual FVector GetSpawnLocation() const;

	/** Called by the timer to spawn a single AI. */
	void TimerCallSpawnAI();

	virtual void SpawnAI(const FVector& SpawnLocation, const FRotator& SpawnRotation);

private:
	/** The timer handle used to spawn the AI */
	FTimerHandle SpawnTimerHandle;

	/** List of AI's that have been spawned */
	TArray<FObjectKey> SpawnedAIs;
};
