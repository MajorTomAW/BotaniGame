// Copyright © 2024 Game Shifters Studio. All rights reserved.

#include "Licensing/GHLicenseManager.h"

#include "GizmoHelperTool.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/Base64.h"

#define AES_BLOCK_SIZE 16


bool FGHLicenseManager::bIsValidLicenseKey = false;
bool FGHLicenseManager::bIsReady = false;
FOnLicenseKeyCheckComplete FGHLicenseManager::WaitLicenseManagerReadyDelegate;

bool EncryptFileContent(const FString& Content, FString& EncodedContent)
{
    // Convert the content string to a byte array
    TArray<uint8> ContentBytes;
    ContentBytes.SetNum(Content.Len());
    for (int32 i = 0; i < Content.Len(); i++)
    {
        ContentBytes[i] = static_cast<uint8>(Content[i]);
    }

    // Base64 encode the byte array
    EncodedContent = FBase64::Encode(ContentBytes);
    return true;
}

bool DecryptFileContent(const FString& EncodedContent, FString& Content)
{
    TArray<uint8> DecodedBytes;
    FBase64::Decode(EncodedContent, DecodedBytes);

    // Convert the byte array back to a string
    Content = TEXT("");
    for (const uint8& Byte : DecodedBytes)
    {
        Content.AppendChar(Byte);
    }

    return true;
}

FString ReadFileContent(const FString& FilePath) 
{
    FString FileContent;
    if (!FFileHelper::LoadFileToString(FileContent, *FilePath))
    {
      UE_LOG(LogGizmoHelperLicensing, Error, TEXT("Failed to read file content"));
      return TEXT("");
    }
  
    FString DecryptedFileContent; 
    DecryptFileContent(FileContent, DecryptedFileContent);
    return DecryptedFileContent;
}

bool SaveFileContent(const FString& FilePath, const FString& Content) 
{
    FString EncryptedContent;
    if (!EncryptFileContent(Content, EncryptedContent))
    {
      return false;
    }
  
    if (!FFileHelper::SaveStringToFile(EncryptedContent, *FilePath))
    {
      return false;
    }
  
    return true;
}

TSharedPtr<FJsonObject> ReadAndParseJson(const FString& FilePath)
{
    const FString FileContent = ReadFileContent(FilePath);
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContent);
    TSharedPtr<FJsonObject> JsonObject;
    FJsonSerializer::Deserialize(Reader, JsonObject);
    return JsonObject;
}

FString GetConfigurationFilePath() 
{
	FString BasePath;

#if PLATFORM_WINDOWS
  // Get the AppData path on Windows
  BasePath = FPlatformMisc::GetEnvironmentVariable(TEXT("APPDATA"));
#elif PLATFORM_MAC
  // Get the Application Support directory path on Mac
  BasePath = FPlatformMisc::GetEnvironmentVariable(TEXT("HOME")) + TEXT("/Library/Application Support");
#endif
	
	BasePath /= TEXT("GizmoHelper");

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*BasePath))
	{
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*BasePath);
	}

	if (BasePath.IsEmpty())
	{
		UE_LOG(LogGizmoHelperLicensing, Error, TEXT("Failed to get license file base path"));
	}

	// Random name for the configuration file
	BasePath /= TEXT("IjogIjk3MEJFOThGNzU0MTQ1RERBRjE5NkVBMzVEQkZDM0Y4IiwKCS");

	if (!FPaths::FileExists(BasePath))
	{
		SaveFileContent(BasePath, TEXT("{}"));
	}

	return BasePath;
}

FString GetDeviceId()
{
    const FString ConfigPath = FGHLicenseManager::GetConfigurationFilePath();
    const TSharedPtr<FJsonObject> JsonObject = FGHLicenseManager::ReadAndParseJson(ConfigPath);

    if (!JsonObject.IsValid() || !JsonObject->HasField(TEXT("DeviceId")))
    {
      UE_LOG(LogGizmoHelperLicensing, Error, TEXT("Failed to get device ID"));
      return TEXT("");
    }

    return JsonObject->GetStringField(TEXT("DeviceId"));
}

bool FGHLicenseManager::CreateDeviceIdIfNotExists()
{
    const FString ConfigPath = GetConfigurationFilePath();
    TSharedPtr<FJsonObject> JsonObject = ReadAndParseJson(ConfigPath);

    if (!JsonObject.IsValid())
    {
		UE_LOG(LogGizmoHelperLicensing, Error, TEXT("Invalid JSON object, creating new device ID"));
		SaveFileContent(ConfigPath, TEXT("{}"));
		JsonObject = ReadAndParseJson(ConfigPath);
	}

	if (!JsonObject.IsValid())
	{
		UE_LOG(LogGizmoHelperLicensing, Error, TEXT("Failed to deserialize JSON object"));
		return false;
	}

	if (JsonObject->HasField(TEXT("DeviceId")))
	{
		UE_LOG(LogGizmoHelperLicensing, Log, TEXT("Device ID already exists"));
		return true;
	}

	const FString UniqueId = FGuid::NewGuid().ToString();
	JsonObject->SetStringField(TEXT("DeviceId"), UniqueId);
	SaveJsonToFile(ConfigPath, JsonObject);
	UE_LOG(LogGizmoHelperLicensing, Log, TEXT("Device ID created successfully"));

	return true;
}

bool FGHLicenseManager::SaveLicenseKey(const FString Key)
{
	const FString ConfigPath = GetConfigurationFilePath();
	TSharedPtr<FJsonObject> JsonObject = ReadAndParseJson(ConfigPath);

	if (!JsonObject.IsValid())
	{
		UE_LOG(LogGizmoHelperLicensing, Error, TEXT("Invalid JSON object, creating new device ID"));
		SaveFileContent(ConfigPath, TEXT("{}"));
		JsonObject = ReadAndParseJson(ConfigPath);
	}

	if (!JsonObject.IsValid())
	{
		UE_LOG(LogGizmoHelperLicensing, Error, TEXT("Failed to deserialize JSON object"));
		return false;
	}

	JsonObject->SetStringField(TEXT("LicenseKey"), Key);
	SaveJsonToFile(ConfigPath, JsonObject);
	UE_LOG(LogGizmoHelperLicensing, Log, TEXT("License key saved successfully"));

	return true;
}

FString FGHLicenseManager::GetLicenseKey()
{
	const FString ConfigPath = GetConfigurationFilePath();
	const FString FileContent = ReadFileContent(ConfigPath);

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContent);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogGizmoHelperLicensing, Error, TEXT("Failed to deserialize JSON object"));
		return TEXT("");
	}

	FString LicenseKey = JsonObject->GetStringField(TEXT("LicenseKey"));
	return LicenseKey;
}


bool FGHLicenseManager::CheckLicenseKey(const FString& LicenseKey, TFunction<void(bool)> Callback)
{
	const FString DeviceId = GetDeviceId();
	const TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	
	HttpRequest->SetURL(FString::Printf(TEXT(R"(https://gameshifers.click/get_license_key_status?license_key=%s&device_id=%s)"), *LicenseKey, *DeviceId));
	HttpRequest->SetVerb("GET");
	
	HttpRequest->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
	{
		bool bIsValid = false;

		/** Throw an error if the request failed */
		if (!bSuccess || Response->GetResponseCode() >= 500)
		{
			UE_LOG(LogGizmoHelperLicensing, Warning, TEXT("Failed to check license key!"));
		}

		if (bSuccess && Response->GetResponseCode() == 200)
		{
			TSharedPtr<FJsonObject> JsonObject;
			const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				const FString Status = JsonObject->GetStringField(TEXT("status"));
				const bool bIsActivated = JsonObject->GetBoolField(TEXT("is_activated"));

				if (Status == "success" && bIsActivated)
				{
					UE_LOG(LogGizmoHelperLicensing, Warning, TEXT("CheckLicenseKey(): License key is valid and activated"));
					bIsValid = true;
				}
			}
			else
			{
				UE_LOG(LogGizmoHelperLicensing, Error, TEXT("Failed to check license key"));
			}
		}
		else
		{
			UE_LOG(LogGizmoHelperLicensing, Error, TEXT("Invalid license key"));
		}

		bIsValidLicenseKey = bIsValid;
		WaitLicenseManagerReadyDelegate.Execute();
		Callback(bIsValidLicenseKey);
	});
	
	HttpRequest->ProcessRequest();
	return true;
}

void FGHLicenseManager::SaveJsonToFile(const FString& FilePath, const TSharedPtr<FJsonObject>& JsonObject)
{
    FString JsonString;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    SaveFileContent(FilePath, JsonString);
}

TSharedPtr<FJsonObject> FGHLicenseManager::ReadAndParseJson(const FString& FilePath)
{
    const FString FileContent = ReadFileContent(FilePath);
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContent);
    TSharedPtr<FJsonObject> JsonObject;
    FJsonSerializer::Deserialize(Reader, JsonObject);
    return JsonObject;
}

FString FGHLicenseManager::GetConfigurationFilePath()
{
    FString BasePath;
#if PLATFORM_WINDOWS
    BasePath = FPlatformMisc::GetEnvironmentVariable(TEXT("APPDATA"));
#elif PLATFORM_MAC
    BasePath = FPlatformMisc::GetEnvironmentVariable(TEXT("HOME")) + TEXT("/Library/Application Support");
#endif

    BasePath /= TEXT("GizmoHelper");

    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*BasePath))
    {
        FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*BasePath);
    }

    if (BasePath.IsEmpty())
    {
        UE_LOG(LogGizmoHelperLicensing, Error, TEXT("Failed to get license file base path"));
        return TEXT("");
    }

    // Random name for the configuration file
    BasePath /= TEXT("IjogIjk3MEJFOThGNzU0MTQ1RERBRjE5NkVBMzVEQkZDM0Y4IiwKCS");
    // BasePath += TEXT(".key"); TODO: Ask @Vadim if this right here is the best practice

    if (!FPaths::FileExists(BasePath))
    {
        SaveFileContent(BasePath, TEXT("{}"));
    }

    return BasePath;
}


