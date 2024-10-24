// Copyright © 2024 MajorT. All rights reserved.

#pragma once

/** Utility struct for global id's used in the playset editor. */
struct FPlaysetEditorIDs
{
	/** Asset category used for the asset definition. */
	static FName PlaysetAssetCategory()		{ static FName ID("Playset"); return ID; }
};

/** Utility struct for built-in browser IDs. */
struct FPlaysetBuiltInBrowserIDs
{
	/** Built-in browser category for favourites. */
	static FName Favourites()				{ static FName ID("Favourites"); return ID; }

	/** Built-in browser category for recent playsets. */
	static FName RecentPlaysets()			{ static FName ID("RecentPlaysets"); return ID; }

	/** Built-in browser category for all playsets. */
	static FName AllPlaysets()				{ static FName ID("AllPlaysets"); return ID; }
};

/** Utility struct providing information for a user category in the playset browser. */
struct FPlaysetCategoryInfo
{
	FPlaysetCategoryInfo(const FText& InDisplayName, const FText& InShortDisplayName, const FSlateIcon& InIcon, FName InUniqueHandle, int32 InSortOrder)
		: DisplayName(InDisplayName)
		, ShortName(InShortDisplayName)
		, Icon(InIcon)
		, UniqueHandle(InUniqueHandle)
		, SortOrder(InSortOrder)
	{
	}

	FPlaysetCategoryInfo(const FText& InDisplayName, const FText& InShortDisplayName, const FName& InIconName, FName InUniqueHandle, int32 InSortOrder)
		: DisplayName(InDisplayName)
		, ShortName(InShortDisplayName)
		, Icon(FAppStyle::GetAppStyleSetName(), InIconName)
		, UniqueHandle(InUniqueHandle)
		, SortOrder(InSortOrder)
	{
	}

	bool operator==(const FPlaysetCategoryInfo& Other) const
	{
		return UniqueHandle == Other.UniqueHandle;
	}
	bool operator!=(const FPlaysetCategoryInfo& Other) const
	{
		return UniqueHandle != Other.UniqueHandle;
	}

	FPlaysetCategoryInfo operator=(const FPlaysetCategoryInfo& Other)
	{
		DisplayName = Other.DisplayName;
		ShortName = Other.ShortName;
		Icon = Other.Icon;
		UniqueHandle = Other.UniqueHandle;
		SortOrder = Other.SortOrder;
		return *this;
	}
	
	/** The Display Name of the category. */
	FText DisplayName;

	/** The short name of the category. */
	FText ShortName;

	/** The icon to use for the category. */
	FSlateIcon Icon;

	/** A unique name for the category. */
	FName UniqueHandle;

	/** The sort order of the category. (lowest first) */
	int32 SortOrder;
};

/** Struct defining a single placeable playset. */
struct FPlaceablePlayset
{
	FPlaceablePlayset()
	{}

	FPlaceablePlayset(const TScriptInterface<IAssetFactoryInterface>& InAssetFactory, const FAssetData& InAssetData, const FString InVirtualPath, TOptional<int32> InSortOrder = TOptional<int32>())
		: AssetFactory(InAssetFactory)
		, AssetData(InAssetData)
		, VirtualPath(InVirtualPath)
		, SortOrder(InSortOrder)
	{
		AutoSetNativeAndDisplayName();
	}

	FPlaceablePlayset(UActorFactory* InFactory, const FAssetData& InAssetData, const FString InVirtualPath, TOptional<int32> InSortOrder = TOptional<int32>())
		: Factory(InFactory)
		, AssetData(InAssetData)
		, VirtualPath(InVirtualPath)
		, SortOrder(InSortOrder)
	{
		AutoSetNativeAndDisplayName();
	}
	
	/** Automatically set the native and display name from the asset. */
	FORCEINLINE void AutoSetNativeAndDisplayName()
	{
		if (const UClass* Class = Cast<UClass>(AssetData.GetAsset()))
		{
			Class->GetName(NativeName);
			DisplayName = Class->GetDisplayNameText();
		}
		else
		{
			NativeName = AssetData.AssetName.ToString();
			DisplayName = FText::FromName(AssetData.AssetName);
		}
	}
	
	/** Return NativeName as an FName and cache it. */
	FORCEINLINE FName GetNativeName() const
	{
		if (NativeFName.IsNone() && !NativeName.IsEmpty())
		{
			NativeFName = FName(*NativeName);
		}

		return NativeFName;
	}

	UActorFactory* Factory = nullptr;
	
	/** The factory used to create an instance of this playset. */
	TScriptInterface<IAssetFactoryInterface> AssetFactory = nullptr;

	/** Asset data for the playset. */
	FAssetData AssetData;

	/** The native name of the playset. */
	FString NativeName;

	/** The path to the asset. */
	FString VirtualPath;

	/** The display name of the playset. */
	FText DisplayName;

	/** Optional sort order (lowest first). */
	TOptional<int32> SortOrder;

private:
	/** The native name as an FName (initialized on access only). */
	mutable FName NativeFName;
};

struct FPlaysetCategory : FPlaysetCategoryInfo
{
	FPlaysetCategory(const FPlaysetCategoryInfo& InInfo)
		: FPlaysetCategoryInfo(InInfo)
	{
	}

	FPlaysetCategory(FPlaysetCategory&& In)
		: FPlaysetCategoryInfo(MoveTemp(In))
		, Items(MoveTemp(In.Items))
	{
	}

	FPlaysetCategory& operator=(FPlaysetCategory&& In)
	{
		FPlaysetCategoryInfo::operator=(MoveTemp(In));
		Items = MoveTemp(In.Items);
		return *this;
	}
	
	TMap<FGuid, TSharedPtr<FPlaceablePlayset>> Items;
};