// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once
class UObject;

BOTANIGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogBotani, Log, All);
BOTANIGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogBotaniMovement, Log, All);
BOTANIGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogBotaniUI, Log, All);
BOTANIGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogBotaniInput, Log, All);
BOTANIGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogBotaniGameFeatures, Log, All);
BOTANIGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogBotaniAbilitySystem, Log, All);
BOTANIGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogBotaniExperience, Log, All);

BOTANIGAME_API FString GetClientServerContextString(UObject* ContextObject = nullptr);

#if PLATFORM_DESKTOP
#define BOTANI_LOG(Verbosity, Format, ...) \
{ \
UE_LOG(LogBotani, Verbosity, Format, ##__VA_ARGS__); \
}

#define BOTANI_MOVEMENT_LOG(Verbosity, Format, ...) \
{ \
UE_LOG(LogBotaniMovement, Verbosity, Format, ##__VA_ARGS__); \
}

#define BOTANI_INPUT_LOG(Verbosity, Format, ...) \
{ \
UE_LOG(LogBotaniInput, Verbosity, Format, ##__VA_ARGS__); \
}

#define BOTANI_GAS_LOG(Verbosity, Format, ...) \
{ \
UE_LOG(LogBotaniAbilitySystem, Verbosity, Format, ##__VA_ARGS__); \
}

#define BOTANI_UI_LOG(Verbosity, Format, ...) \
{ \
UE_LOG(LogBotaniUI, Verbosity, Format, ##__VA_ARGS__); \
}

#define BOTANI_EXP_LOG(Verbosity, Format, ...) \
{ \
UE_LOG(LogBotaniExperience, Verbosity, Format, ##__VA_ARGS__); \
}

#define BOTANI_GFP_LOG(Verbosity, Format, ...) \
{ \
UE_LOG(LogBotaniGameFeatures, Verbosity, Format, ##__VA_ARGS__); \
}
#endif