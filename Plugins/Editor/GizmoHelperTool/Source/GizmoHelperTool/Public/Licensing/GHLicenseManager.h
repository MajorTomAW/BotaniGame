// Copyright © 2024 Game Shifters Studio. All rights reserved.

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Misc/Paths.h"

DECLARE_DELEGATE(FOnLicenseKeyCheckComplete);

class FGHLicenseManager
{
public:
    static bool CreateDeviceIdIfNotExists();
    
    static bool SaveLicenseKey(const FString Key);

    static FString GetLicenseKey();

	static bool CheckLicenseKey(const FString& LicenseKey,  TFunction<void(bool)> Callback);

    static bool bIsValidLicenseKey;
	static bool bIsReady;
    
    //~ Begin License Utility Functions
    static void SaveJsonToFile(const FString& FilePath, const TSharedPtr<FJsonObject>& JsonObject);
    static TSharedPtr<FJsonObject> ReadAndParseJson(const FString& FilePath);
    static FString GetConfigurationFilePath();
    //~ End License Utility Functions, these functions are straight from @developd since they are good to go, and rewriting them would be a waste of time
	
	static FOnLicenseKeyCheckComplete WaitLicenseManagerReadyDelegate;
};
