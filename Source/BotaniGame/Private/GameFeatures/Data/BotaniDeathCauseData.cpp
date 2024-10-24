// Copyright © 2024 Botanibots Team. All rights reserved.


#include "GameFeatures/Data/BotaniDeathCauseData.h"

FBotaniDeathCauseInfo UBotaniDeathCauseData::GetDeathCauseInfo(const FGameplayTag& DeathCauseTag) const
{
	const FBotaniDeathCauseInfo FoundInfo = *DeathCauseInfos.FindByPredicate([&DeathCauseTag](const FBotaniDeathCauseInfo& Info)
	{
		return Info.DeathCauseTag.MatchesTagExact(DeathCauseTag);
	});

	return FoundInfo;
}
