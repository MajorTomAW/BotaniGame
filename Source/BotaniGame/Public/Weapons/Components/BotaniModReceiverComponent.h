// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BotaniModReceiverComponent.generated.h"

/**
 * UBotaniModReceiverComponent
 *
 * An component used to take track of different modification assets that can be applied to the owner.
 * The Mod Receiver will manage active modifications and apply their effects to the owner.
 * Each active modification will have a unique ID that can be used to find and remove it.
 */
UCLASS()
class BOTANIGAME_API UBotaniModReceiverComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:
protected:
	//~ Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface
	
private:
};
