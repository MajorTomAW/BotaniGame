// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Game/BotaniWorldSettings.h"

#include "BotaniLogChannels.h"
#include "EngineUtils.h"
#include "Engine/AssetManager.h"
#include "GameFramework/PlayerStart.h"
#include "Misc/UObjectToken.h"

ABotaniWorldSettings::ABotaniWorldSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	bForceStandaloneNetMode = false;
#endif
}

#if WITH_EDITOR
void ABotaniWorldSettings::CheckForErrors()
{
	Super::CheckForErrors();

	FMessageLog MapCheck("MapCheck");

	for (TActorIterator<APlayerStart> PlayerStartIt(GetWorld()); PlayerStartIt; ++PlayerStartIt)
	{
		APlayerStart* PlayerStart = *PlayerStartIt;

		if (IsValid(PlayerStart) && PlayerStart->GetClass() == APlayerStart::StaticClass())
		{
			MapCheck.Warning()
				->AddToken(FUObjectToken::Create(PlayerStart))
				->AddToken(FTextToken::Create(FText::FromString("is a normal APlayerStart, replace with ABotaniPlayerStart.")));
		}
	}
}
#endif

FPrimaryAssetId ABotaniWorldSettings::GetDefaultGameplayExperience() const
{
	FPrimaryAssetId Result;

	if (DefaultGameplayExperience.IsNull())
	{
		return Result;
	}

	Result = UAssetManager::Get().GetPrimaryAssetIdForPath(DefaultGameplayExperience.ToSoftObjectPath());

	if (!Result.IsValid())
	{
		BOTANI_EXP_LOG(Error, TEXT("%s.DefaultGameplayExperience is %s but that failed to resolve into an asset ID (you might need to add a path to the Asset Rules in your game feature plugin or project settings"),
			*GetPathNameSafe(this), *DefaultGameplayExperience.ToString());
	}

	return Result;
}
