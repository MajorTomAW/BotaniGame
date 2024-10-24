// Copyright 2022 Ares9323. All Rights Reserved.

#include "RestartEditor.h"
#include "RestartEditorStyle.h"
#include "RestartEditorCommands.h"
#include "ToolMenus.h"

#include "Interfaces/IMainFrameModule.h"
#include "LevelEditor.h"

static const FName RestartEditorTabName("RestartEditor");

#define LOCTEXT_NAMESPACE "FRestartEditorModule"

void FRestartEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FRestartEditorStyle::Initialize();
	FRestartEditorStyle::ReloadTextures();

	FRestartEditorCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FRestartEditorCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FRestartEditorModule::PluginButtonClicked),
		FCanExecuteAction());

	IMainFrameModule& mainFrame = FModuleManager::Get().LoadModuleChecked<IMainFrameModule>("MainFrame");
	mainFrame.GetMainFrameCommandBindings()->Append(PluginCommands.ToSharedRef());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FRestartEditorModule::RegisterMenus));
}

void FRestartEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FRestartEditorStyle::Shutdown();

	FRestartEditorCommands::Unregister();
}

void FRestartEditorModule::PluginButtonClicked()
{
	FUnrealEdMisc::Get().RestartEditor(false);
}

static void RestartButton_Clicked()
{
	FUnrealEdMisc::Get().RestartEditor(false);
}

static bool HasPlayWorld()
{
	return GEditor->PlayWorld != nullptr;
}

static bool HasNoPlayWorld()
{
	return !HasPlayWorld();
}

void FRestartEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);
	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		FToolMenuSection& Section = ToolbarMenu->AddSection("PlayGameExtensions", TAttribute<FText>(), FToolMenuInsert("Play", EToolMenuInsertType::After));
		
		FToolMenuEntry RestartEditorEntry = FToolMenuEntry::InitToolBarButton(
				"RestartEditor",
				FUIAction(
				FExecuteAction::CreateStatic(&RestartButton_Clicked),
				FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
				FIsActionChecked(),
				FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)),
				LOCTEXT("RestartEditorButton", "Restart Editor"),
				LOCTEXT("RestartEditorDescription", "Restart the editor to apply changes made to the project."),
				FSlateIcon(FRestartEditorStyle::GetStyleSetName(), "RestartEditor.PluginAction"));

		RestartEditorEntry.StyleNameOverride = "CalloutToolbar";
		Section.AddEntry(RestartEditorEntry).SetCommandList(PluginCommands);
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRestartEditorModule, RestartEditor)