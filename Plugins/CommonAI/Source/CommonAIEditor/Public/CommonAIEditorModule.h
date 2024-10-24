// Copyright 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FWorkflowCentricApplication;

/** Namespace to hold all global identifiers for the CommonAIEditor module. */
namespace CommonAIEditor
{
	static const FName GraphEditorID = FName(TEXT("AIDecisionRule_GraphEditorID"));	
}

/**
 * The public interface to this module.  In most cases, this interface is only public to sibling modules
 * within this plugin.
 */
class ICommonAIEditorModule : public IModuleInterface
{
public:
	/**
	 * Singleton-like access to this module's interface. This is just for convenience!
	 * Don't use it for anything that can be better placed in an instance.
	 *
	 * @returns Returns a singleton instance, loading the module on demand if needed
	 */
	static ICommonAIEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<ICommonAIEditorModule>("CommonAIEditor");
	}

	/**
	 * Checks to see if this module is loaded and ready. It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @returns True if the module is loaded and ready to use
	 */
	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("CommonAIEditor");
	}

	virtual TSharedRef<FWorkflowCentricApplication> CreateAIDecisionEditorApp(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* Object) = 0;

	/** AI Decision Rule app identifier string */
	static const FName AIDecisionEditorAppIdentifier;
};