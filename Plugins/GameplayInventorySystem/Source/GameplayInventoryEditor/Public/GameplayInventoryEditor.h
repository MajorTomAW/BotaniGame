#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FWorkflowCentricApplication;

/** Namespace that stores global IDs for the editor. */
namespace ItemEditorIDs
{
    static const FName ItemEditorApp = "ID_ItemEditorApp";
    
    namespace Modes
    {
        static const FName ItemMode = "ID_MODE_Item";
        static const FName EquipmentMode = "ID_MODE_Equipment";
    }

    namespace Tabs
    {
        static const FName ItemTab_Details = "ID_TAB_ItemDetails";
        static const FName ItemTab_Fragments = "ID_TAB_ItemFragments";
    }
}

/** The public interface to this module. */
class IGameplayInventoryEditorModule : public IModuleInterface
{
public:
    /**
     * Singleton-like access to this module's interface. This is just for convenience!
     * Beware of calling this during the shutdown phase, though. Your module might have been unloaded already.
     *
     * @return Singleton instance, loading the module on demand if needed.
     */
    static inline IGameplayInventoryEditorModule& Get()
    {
        return FModuleManager::LoadModuleChecked<IGameplayInventoryEditorModule>("GameplayInventoryEditor");
    }

    /**
     * Checks to see if this module is loaded and ready. It is only valid to call Get() if IsAvailable() returns true.
     *
     * @return True if the module is loaded and ready to use.
     */
    static inline bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("GameplayInventoryEditor");
    }

    /**
     * Creates a new instance of the inventory editor application.
     */
    virtual TSharedRef<FWorkflowCentricApplication> CreateItemEditorApp(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* Object) = 0;
};
