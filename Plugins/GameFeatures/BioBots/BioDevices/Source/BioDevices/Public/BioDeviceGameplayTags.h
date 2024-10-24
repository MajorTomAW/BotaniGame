// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "NativeGameplayTags.h"

namespace BioDeviceGameplayTags
{
	namespace Request
	{
		BIODEVICES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Request_EnableDevice);
		BIODEVICES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Request_DisableDevice);
	}

	namespace State
	{
		BIODEVICES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_DeviceActive);
		BIODEVICES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_DeviceInactive);
	}

	namespace Message
	{
		BIODEVICES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Message_DeviceNowEnabled);
		BIODEVICES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Message_DeviceNowDisabled);
	}
}