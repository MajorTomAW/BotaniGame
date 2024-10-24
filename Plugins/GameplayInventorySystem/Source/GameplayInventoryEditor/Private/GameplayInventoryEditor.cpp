// Copyright © 2024 MajorT. All rights reserved.

#include "GameplayInventoryEditor.h"

#include "Application/ItemEditorApp.h"

#define LOCTEXT_NAMESPACE "GameplayInventoryEditor"

class FGameplayInventoryEditorModule final : public IGameplayInventoryEditorModule
{
public:
	//~ Begin IModuleInterface Interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface Interface

	virtual TSharedRef<FWorkflowCentricApplication> CreateItemEditorApp(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* Object) override;

protected:
	void OnPostEngineInit();
};
IMPLEMENT_MODULE(FGameplayInventoryEditorModule, GameplayInventoryEditor)

void FGameplayInventoryEditorModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FGameplayInventoryEditorModule::OnPostEngineInit);
}

void FGameplayInventoryEditorModule::OnPostEngineInit()
{
}

void FGameplayInventoryEditorModule::ShutdownModule()
{
	IGameplayInventoryEditorModule::ShutdownModule();
}

TSharedRef<FWorkflowCentricApplication> FGameplayInventoryEditorModule::CreateItemEditorApp(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* Object)
{
	TSharedRef<FItemEditorApp> App(new FItemEditorApp());
	App->InitItemEditorApp(Mode, InitToolkitHost, Object);
	return App;
}

