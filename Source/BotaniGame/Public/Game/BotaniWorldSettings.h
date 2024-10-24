// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "BotaniWorldSettings.generated.h"

/**
 * The default world settings object, used primarily to set the default gameplay experience to use when playing on this map
 */
UCLASS()
class BOTANIGAME_API ABotaniWorldSettings : public AWorldSettings
{
	GENERATED_UCLASS_BODY()

public:
#if WITH_EDITOR
	virtual void CheckForErrors() override;
#endif
#if WITH_EDITORONLY_DATA
	/**
	 * Forces standalone net mode for this level when PIE is launched
	 * Used for front-end or other standalone experiences.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "PIE")
	uint8 bForceStandaloneNetMode:1;
#endif

	/** Returns the default experience converted to a primary asset id */
	FPrimaryAssetId GetDefaultGameplayExperience() const;
	
protected:
	/** The default experience to use when a server opens this map if it is not overridden by the user-facing experience */
	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	TSoftClassPtr<class UBotaniExperienceDefinition> DefaultGameplayExperience;
};
