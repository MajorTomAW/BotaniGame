﻿// Copyright Epic Games, Inc. All Rights Reserved.

#include "AIUtilities.h"

#define LOCTEXT_NAMESPACE "AIUtilitiesModule"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(AIUtilityTags::InputProvider::TAG_InputProvider, "AI.Utility.InputProvider", "Base input provider tag.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(AIUtilityTags::InputProvider::TAG_InputProvider_Distance, "AI.Utility.InputProvider.Distance", "Distance input provider.");

void FAIUtilitiesModule::StartupModule()
{
}

void FAIUtilitiesModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAIUtilitiesModule, AIUtilities)