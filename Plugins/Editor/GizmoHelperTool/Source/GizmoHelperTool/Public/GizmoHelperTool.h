// Copyright © 2024 Game Shifters Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#pragma region LogCategories
DECLARE_LOG_CATEGORY_EXTERN(LogGizmoHelperLicensing, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogGizmoHelperUtils, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogGizmoHelper, Log, All);

#if IS_GIZMO_DEBUG
DECLARE_LOG_CATEGORY_EXTERN(LogGizmoDebug, Log, All);
#endif

#if PLATFORM_DESKTOP
#define GIZMO_LOG(Verbosity, Format, ...) \
{ \
UE_LOG(LogGizmoHelper, Verbosity, Format, ##__VA_ARGS__); \
}

#define GIZMO_LOG_LICENSING(Verbosity, Format, ...) \
{ \
UE_LOG(LogGizmoHelperLicensing, Verbosity, Format, ##__VA_ARGS__); \
}
#endif
#pragma endregion

class FGizmoHelperToolModule : public IModuleInterface
{
public:
	//~ Begin IModuleInterface Interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface Interface
	
	virtual bool CheckAndShowLicensePopup();

protected:
	static bool CheckInternetConnection(TFunction<void(bool)> Callback);
	void OnCheckInternetConnectionComplete(bool bSuccess);

private:
	TWeakPtr<SWindow> PopupWindowPtr;
	TSharedPtr<STextBlock> MessageTextBlockPtr;
	FString GlobalSettingsFile;
};
