// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "BotaniWeaponStateComponent.generated.h"

/**
 * UBotaniWeaponStateComponent
 *
 * Tracks weapon state and recently confirmed hit markers.
 */
UCLASS()
class  UBotaniWeaponStateComponent : public UControllerComponent
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface
	
protected:
private:
};
