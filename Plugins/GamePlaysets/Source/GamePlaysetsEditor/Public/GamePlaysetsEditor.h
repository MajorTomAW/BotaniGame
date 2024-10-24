#pragma once

#include "CoreMinimal.h"
#include "PlaysetEditorIDs.h"

class IGamePlaysetsEditorModule : public IModuleInterface
{
public:
	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 * 
	 * @return Singleton instance, loading the module on demand if needed 
	 */
	static IGamePlaysetsEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<IGamePlaysetsEditorModule>("GamePlaysetsEditor");
	}

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 * 
	 * @return True if the module is loaded and ready to use
	 */
	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("GamePlaysetsEditor");
	}

	/**
	 * Get all playset browser categories, sorted by their sort order
	 *
	 * @param OutCategories The output array of categories
	 */
	virtual void GetSortedCategories(TArray<FPlaysetCategoryInfo>& OutCategories) = 0;

	/**
	 * Register a new category of playsets
	 * @param Info	Information about the category to register
	 * @return true on success, false on failure (probably if the category's unique handle is already in use)
	 */
	virtual bool RegisterPlaysetBrowserCategory(const FPlaysetCategoryInfo& Info) = 0;

	/**
	 * Get all items in a given category
	 *
	 * @param Category	The unique name of the category to get items for
	 * @param OutItems	[out] Will be filled with the list of items
	 */
	virtual void GetItemsForCategory(FName Category, TArray<TSharedPtr<FPlaceablePlayset>>& OutItems) const = 0;

	/**
	 * Instruct the category associated with the given unique handle to regenerate its items
	 * @param Category	The unique name of the category to regenerate items for
	 */
	virtual void RegenerateItemsForCategory(FName Category) = 0;

public:
	DECLARE_EVENT(IGamePlaysetsEditorModule, FOnPlaysetBrowserCategoriesChanged);
	virtual FOnPlaysetBrowserCategoriesChanged& OnPlaysetBrowserCategoriesChanged() = 0;

	DECLARE_EVENT(IGamePlaysetsEditorModule, FOnPlaysetItemFilteringChanged);
	virtual FOnPlaysetItemFilteringChanged& OnPlaysetItemFilteringChanged() = 0;

	DECLARE_EVENT_OneParam(IGamePlaysetsEditorModule, FOnPlaysetBrowserCategoryRefreshed, FName /* Category Unique Handle */);
	virtual FOnPlaysetBrowserCategoryRefreshed& OnPlaysetBrowserCategoryRefreshed() = 0;

protected:
	TMap<FName, FPlaysetCategory> Categories;
	TArray<FAssetData> PlaysetCache;
};
