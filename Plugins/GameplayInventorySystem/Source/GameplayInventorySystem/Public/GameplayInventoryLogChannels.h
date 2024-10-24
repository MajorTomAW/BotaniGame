// Copyright © 2024 MajorT. All rights reserved.

#pragma once

GAMEPLAYINVENTORYSYSTEM_API DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All);

#if PLATFORM_DESKTOP
#define LOG_INVENTORY(Verbosity, Format, ...) UE_LOG(LogInventory, Verbosity, Format, ##__VA_ARGS__)
#endif
