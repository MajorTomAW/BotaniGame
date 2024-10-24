// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "BotaniPathFollowingComponent.generated.h"

/**
 * UBotaniPathFollowingComponent
 *
 * An actor component class.
 */
UCLASS()
class BOTANIAI_API UBotaniPathFollowingComponent : public UPathFollowingComponent
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Pathfinding)
	float OffsetFromCornersDistance;
	
protected:
	//~ Begin UPathFollowingComponent interface
	virtual FAIRequestID RequestMove(const FAIMoveRequest& RequestData, FNavPathSharedPtr InPath) override;
	virtual void FollowPathSegment(float DeltaTime) override;
	//~ End UPathFollowingComponent interface
};
