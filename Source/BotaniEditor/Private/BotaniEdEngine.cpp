// Copyright © 2024 Botanibots Team. All rights reserved.


#include "BotaniEdEngine.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Game/BotaniWorldSettings.h"
#include "Settings/ContentBrowserSettings.h"
#include "Widgets/Notifications/SNotificationList.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniEdEngine)

#define LOCTEXT_NAMESPACE "BotaniEditor"


UBotaniEdEngine::UBotaniEdEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bFirstTickSetup = false;
}

void UBotaniEdEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);
	FirstTickSetup();
}

void UBotaniEdEngine::Start()
{
	Super::Start();
}

void UBotaniEdEngine::Tick(float DeltaSeconds, bool bIdleMode)
{
	Super::Tick(DeltaSeconds, bIdleMode);
}

FGameInstancePIEResult UBotaniEdEngine::PreCreatePIEInstances(const bool bAnyBlueprintErrors, const bool bStartInSpectatorMode, const float PIEStartTime, const bool bSupportsOnlinePIE, int32& InNumOnlinePIEInstances)
{
	if (const ABotaniWorldSettings* BotaniWorldSettings = Cast<ABotaniWorldSettings>(EditorWorld->GetWorldSettings()))
	{
		if (BotaniWorldSettings->bForceStandaloneNetMode)
		{
			EPlayNetMode OutPlayNetMode;
			PlaySessionRequest->EditorPlaySettings->GetPlayNetMode(OutPlayNetMode);

			if (OutPlayNetMode != PIE_Standalone)
			{
				PlaySessionRequest->EditorPlaySettings->SetPlayNetMode(PIE_Standalone);

				FNotificationInfo Info(LOCTEXT("PIEStandaloneMode", "Forcing NetMode: Standalone Mode"));
				Info.ExpireDuration = 2.0f;
				FSlateNotificationManager::Get().AddNotification(Info);
			}
		}
	}
	
	FGameInstancePIEResult Result = Super::PreCreatePIEInstances(bAnyBlueprintErrors, bStartInSpectatorMode, PIEStartTime, bSupportsOnlinePIE,InNumOnlinePIEInstances);
	return Result;
}

void UBotaniEdEngine::FirstTickSetup()
{
	if (bFirstTickSetup)
	{
		return;
	}

	bFirstTickSetup = true;

	// Force show plugin content on the first load.
	GetMutableDefault<UContentBrowserSettings>()->SetDisplayPluginFolders(true);
}

#undef LOCTEXT_NAMESPACE