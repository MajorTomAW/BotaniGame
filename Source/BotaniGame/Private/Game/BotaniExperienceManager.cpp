// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Game/BotaniExperienceManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniExperienceManager)

#if WITH_EDITOR
void UBotaniExperienceManager::OnPlayInEditorBegun()
{
	ensure(GameFeaturePluginRequestCountMap.IsEmpty());
	GameFeaturePluginRequestCountMap.Empty();
}

void UBotaniExperienceManager::NotifyOfPluginActivation(const FString PluginURL)
{
	if (!GIsEditor)
	{
		return;
	}
	
	UBotaniExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<UBotaniExperienceManager>();
	check(ExperienceManagerSubsystem);

	// Track the number of requesters who activate this plugin. Multiple load/activation requests are always allowed because concurrent requests are handled.
	int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindOrAdd(PluginURL);
	++Count;
}

bool UBotaniExperienceManager::RequestToDeactivatePlugin(const FString PluginURL)
{
	if (!GIsEditor)
	{
		return true;
	}

	UBotaniExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<UBotaniExperienceManager>();
	check(ExperienceManagerSubsystem);

	// Only let the last requester to get this far deactivate the plugin
	int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindChecked(PluginURL);
	--Count;

	if (Count == 0)
	{
		ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.Remove(PluginURL);
		return true;
	}

	return false;
}
#endif