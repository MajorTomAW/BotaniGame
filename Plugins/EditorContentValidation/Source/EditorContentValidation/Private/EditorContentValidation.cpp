// Copyright Epic Games, Inc. All Rights Reserved.

#include "EditorContentValidation.h"

#include "ValidatorToolbarButton.h"
#include "EditorContentValidation/Validation/EditorValidator.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "FEditorContentValidationModule"

DEFINE_LOG_CATEGORY(LogEditorContentValidation);

static bool HasPlayWorld()
{
	return GEditor->PlayWorld != nullptr;
}

static bool HasNoPlayWorld()
{
	return !HasPlayWorld();
}

//////////////////////////////////////////////////////////////////////////
/// FEditorContentValidationModule

class FEditorContentValidationModule : public IModuleInterface
{
public:
	//~ Begin IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface interface

protected:
	typedef FEditorContentValidationModule ThisClass;

	static void RegisterEditorToolbarMenus();
	static void CheckGameContent();

private:
	FDelegateHandle ToolMenusHandle;
};

void FEditorContentValidationModule::StartupModule()
{
	FEditorValidatorStyle::Initialize();
	
	if (!IsRunningGame())
	{
		if (FSlateApplication::IsInitialized())
		{
			ToolMenusHandle = UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&RegisterEditorToolbarMenus));
		}
	}
}

void FEditorContentValidationModule::ShutdownModule()
{
	if (UObjectInitialized() && ToolMenusHandle.IsValid())
	{
		UToolMenus::UnRegisterStartupCallback(ToolMenusHandle);
	}
}

void FEditorContentValidationModule::RegisterEditorToolbarMenus()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuSection& Section = Menu->AddSection("PlayGameExtensions", TAttribute<FText>(), FToolMenuInsert("Play", EToolMenuInsertType::After));

	FToolMenuEntry CheckContentEntry = FValidatorToolbarButton::GetValidatorToolbarButton(FExecuteAction::CreateStatic(&CheckGameContent));
	CheckContentEntry.StyleNameOverride = "CalloutToolbar";
	
	Section.AddEntry(CheckContentEntry);
}

void FEditorContentValidationModule::CheckGameContent()
{
	UEditorValidator::ValidateCheckedOutContent(true, EDataValidationUsecase::Manual);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEditorContentValidationModule, EditorContentValidation)