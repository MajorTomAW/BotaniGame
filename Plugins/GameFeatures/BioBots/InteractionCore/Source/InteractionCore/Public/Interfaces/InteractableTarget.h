// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractionOption.h"
#include "UObject/Interface.h"
#include "InteractableTarget.generated.h"

struct FBotaniInteractionQuery;
class IInteractableTarget;

/** A builder for interaction options for botani characters. */
class FBotaniInteractionOptionBuilder
{
public:
	FBotaniInteractionOptionBuilder(const TScriptInterface<IInteractableTarget>& InterfaceTargetScope, TArray<FBotaniInteractionOption>& InteractionOptions)
		: Scope(InterfaceTargetScope)
		, Options(InteractionOptions)
	{
	}

	void AddInteractionOption(const FBotaniInteractionOption& Option) const
	{
		FBotaniInteractionOption& OptionEntry = Options.Add_GetRef(Option);
		OptionEntry.InteractableTarget = Scope;
	}

private:
	TScriptInterface<IInteractableTarget> Scope;
	TArray<FBotaniInteractionOption>& Options;
};


UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UInteractableTarget : public UInterface
{
	GENERATED_BODY()
};

/** Interface for interactable targets. */
class IInteractableTarget
{
	GENERATED_BODY()

public:
	/** Called to gather all the possible interaction options for the given query. */
	virtual void GatherInteractionOptions(const FBotaniInteractionQuery& InteractionQuery, FBotaniInteractionOptionBuilder& OptionBuilder) = 0;

	/** Called to customize the interaction event data for the given interaction event tag. */
	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) { }
};
