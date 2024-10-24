// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "BioPlayerSpawningManager.generated.h"

/**
 * UBioPlayerSpawningManager
 *
 * A player spawning manager for BioDevices
 */
UCLASS()
class BIODEVICES_API UBioPlayerSpawningManager : public UGameStateComponent
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UActorComponent Interface
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface
protected:
private:
	virtual void OnLevelAdded(ULevel* InLevel, UWorld* InWorld);
	virtual void HandleOnActorSpawned(AActor* Actor);
	
	virtual AActor* DeterminePlayerStartSpot(AController* Player);

private:
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<class ADevice_BioPlayerStart>> PlayerStartDevices;
};
