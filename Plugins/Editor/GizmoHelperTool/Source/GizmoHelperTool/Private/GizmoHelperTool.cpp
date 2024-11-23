// Copyright © 2024 Game Shifters Studio. All rights reserved.

#include "GizmoHelperTool.h"

#include "GHDeveloperSettings.h"
#include "Licensing/GHLicenseManager.h"
#include "Widgets/SWindow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableText.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IPluginManager.h"


#define LOCTEXT_NAMESPACE "FGizmoHelperToolModule"

#pragma region LogCategories
DEFINE_LOG_CATEGORY(LogGizmoHelperLicensing);
DEFINE_LOG_CATEGORY(LogGizmoHelperUtils);
DEFINE_LOG_CATEGORY(LogGizmoHelper);

#if IS_GIZMO_DEBUG
DEFINE_LOG_CATEGORY(LogGizmoDebug);
#endif
#pragma endregion

bool FGizmoHelperToolModule::CheckInternetConnection(TFunction<void(bool)> Callback)
{
	const TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT(R"(https://google.com)")));
	HttpRequest->SetVerb("GET");
	HttpRequest->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
	{
		Callback(bSuccess && Response->GetResponseCode() == 200);
	});
	
	HttpRequest->ProcessRequest();
	return true;
}

void FGizmoHelperToolModule::OnCheckInternetConnectionComplete(bool bSuccess)
{
	if (bSuccess)
	{
		FGizmoHelperToolModule::CheckAndShowLicensePopup();
		return;
	}
	UE_LOG(LogGizmoHelperLicensing, Error, TEXT("No internet connection!"));
}

void FGizmoHelperToolModule::StartupModule()
{
	FString GlobalSettingsPath = IPluginManager::Get().FindPlugin(TEXT("GizmoHelperTool"))->GetBaseDir();
	GlobalSettingsPath /= "Settings.ini";
	GlobalSettingsFile = FConfigCacheIni::NormalizeConfigIniPath(GlobalSettingsPath);

	if (UGHDeveloperSettings::Get()->bGlobalSettings)
	{
		if (FPaths::FileExists(GlobalSettingsFile))
		{
			UGHDeveloperSettings::Get()->LoadConfig(nullptr, *GlobalSettingsFile);
		}
	}
	
	CheckInternetConnection([this](bool bSuccess) { this->OnCheckInternetConnectionComplete(bSuccess); });
}

bool FGizmoHelperToolModule::CheckAndShowLicensePopup()
{
	UE_LOG(LogGizmoHelperLicensing, Log, TEXT("Checking license key"));
	FGHLicenseManager::CreateDeviceIdIfNotExists();

	const FString SavedLicenseKey = FGHLicenseManager::GetLicenseKey();

	TSharedPtr<SEditableTextBox> EditableTextBox;
	TSharedPtr<STextBlock> MessageTextBlock;

	TSharedRef<SWindow> SPopupWindow = SNew(SWindow)
		.Title(FText::FromString(TEXT("Gizmo Helper Tool: Enter the license key")))
		.ClientSize(FVector2D(500, 100))
		.Content()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10)
			[
				SAssignNew(EditableTextBox, SEditableTextBox)
				
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(10, 0, 10, 0))
			[
				SAssignNew(MessageTextBlock, STextBlock)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("Submit")))
				.OnClicked_Lambda([this, EditableTextBox, MessageTextBlock]()
				{
					// Save the license code to the developer settings
					FText LicenseCode = EditableTextBox->GetText();
					FGHLicenseManager::CheckLicenseKey(LicenseCode.ToString(), [this, LicenseCode, MessageTextBlock](bool bIsValidLicenseKey) 
					{
						if (bIsValidLicenseKey)
						{
							FGHLicenseManager::SaveLicenseKey(LicenseCode.ToString());
							if (PopupWindowPtr.IsValid())
							{
								PopupWindowPtr.Pin()->RequestDestroyWindow();
							}
						}
						else 
						{
							if (MessageTextBlock.IsValid())
							{
								MessageTextBlock->SetText(FText::FromString(TEXT("*This key is not valid")));
							}
						}
					});

					return FReply::Handled();
				})
			]
		];
	
	// Assign the weak pointer
	PopupWindowPtr = SPopupWindow;
	MessageTextBlockPtr = MessageTextBlock;

	FGHLicenseManager::CheckLicenseKey(SavedLicenseKey, [this, SPopupWindow](bool bIsLicenseKeyValid) 
	{
		FGHLicenseManager::bIsValidLicenseKey = bIsLicenseKeyValid;
		if (!bIsLicenseKeyValid)
		{
			// Show the popup window
			FSlateApplication::Get().AddWindow(SPopupWindow);
		}
	});
	return true;
}



void FGizmoHelperToolModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGizmoHelperToolModule, GizmoHelperTool)