// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagStackContainer.h"
#include "GameplayTagStackInterface.h"
#include "Components/GameplayInventoryComponent.h"
#include "Components/GameplayInventoryManager.h"
#include "UObject/Object.h"
#include "GameplayInventoryItemInstance.generated.h"

/**
 * UGameplayInventoryItemInstance
 * 
 * Represents a replicated instance of an item in the inventory system.
 */
UCLASS(BlueprintType, Blueprintable, MinimalAPI, Config = Game)
class UGameplayInventoryItemInstance : public UObject, public IGameplayTagStackInterface
{
	GENERATED_UCLASS_BODY()
	friend class UGameplayInventoryManager;

public:
	//~ Begin UObject Interface
	GAMEPLAYINVENTORYSYSTEM_API virtual UWorld* GetWorld() const override;
	GAMEPLAYINVENTORYSYSTEM_API virtual bool IsSupportedForNetworking() const override { return true; }
	GAMEPLAYINVENTORYSYSTEM_API virtual bool CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack) override;
	GAMEPLAYINVENTORYSYSTEM_API virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	//~ End UObject Interface

	//~ Begin IGameplayTagStacksInterface
	GAMEPLAYINVENTORYSYSTEM_API virtual const FGameplayTagStackContainer& GetOwnedGameplayTagStacks() const override;
	//~ End IGameplayTagStacksInterface

	/** Returns the owning inventory component */
	template <typename T>
	T* GetOwnerComponent() const;
	UGameplayInventoryManager* GetOwnerComponent() const { return GetOwnerComponent<UGameplayInventoryManager>(); }

	/** Called to set the item definition for this instance */
	GAMEPLAYINVENTORYSYSTEM_API virtual void SetItemDef(UGameplayInventoryItemDefinition* InItemDef);

	/** Called when a new instance has been created, also called on clients */
	GAMEPLAYINVENTORYSYSTEM_API virtual void OnInstanceCreated(const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext* InItemContext = nullptr);

	/** Called when a new instance has been created, also called on clients */
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory|Item", meta = (DisplayName = "On Instance Created"))
	GAMEPLAYINVENTORYSYSTEM_API void K2_OnInstanceCreated(const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext& InItemContext);

	/** Called when the item instance has been removed from the inventory */
	GAMEPLAYINVENTORYSYSTEM_API virtual void OnInstanceRemoved(const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext* InItemContext = nullptr);

	/** Called when the item instance has been removed from the inventory */
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory|Item", meta = (DisplayName = "On Instance Removed"))
	GAMEPLAYINVENTORYSYSTEM_API void K2_OnInstanceRemoved(const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext& InItemContext);

	/** Called when the item instance has changed */
	GAMEPLAYINVENTORYSYSTEM_API virtual void OnInstanceChanged(const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext* InItemContext = nullptr);

public:
	/** Returns the stack count of this instance in the inventory */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Item")
	GAMEPLAYINVENTORYSYSTEM_API virtual int32 GetCurrentStackCount() const;

	/** Returns the display name of the item. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Item")
	GAMEPLAYINVENTORYSYSTEM_API FText GetDisplayName() const;

	/** returns the item icons for this instance */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Item")
	GAMEPLAYINVENTORYSYSTEM_API UTexture2D* GetIcon(const bool bUseLargeIfNotFound = true) const;

	/** Returns the gameplay tags owned by the item definition */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Item")
	GAMEPLAYINVENTORYSYSTEM_API FGameplayTagContainer GetConstGameplayTags() const;

	/** Returns the item definition associated with this instance */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Item")
	GAMEPLAYINVENTORYSYSTEM_API virtual UGameplayInventoryItemDefinition* GetItemDefinition() const { return ItemDefinition; }

	/** Returns the typed item definition associated with this instance */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Item", meta = (DeterminesOutputType = "ItemDefinitionType"))
	GAMEPLAYINVENTORYSYSTEM_API virtual UGameplayInventoryItemDefinition* GetItemDefinitionByType(TSubclassOf<UGameplayInventoryItemDefinition> ItemDefinitionType) const;

	/** Returns the owner actor of this item instance */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Item")
	GAMEPLAYINVENTORYSYSTEM_API virtual AActor* GetOwnerActor() { return GetOwningActor(); }

	/** Returns the owning inventory manager of this item instance */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Item")
	GAMEPLAYINVENTORYSYSTEM_API virtual UGameplayInventoryManager* GetOwnerInventoryManager() { return GetOwnerComponent(); }

	/** Returns the handle of the item spec for this instance */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Item")
	GAMEPLAYINVENTORYSYSTEM_API virtual FGameplayInventoryItemSpecHandle GetItemSpecHandle() const { return CurrentSpecHandle; }

	/** Returns an item fragment of the given type */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Item", meta = (DeterminesOutputType = "FragmentClass"))
	GAMEPLAYINVENTORYSYSTEM_API virtual class UGameplayInventoryItemFragment* FindItemFragmentByClass(TSubclassOf<UGameplayInventoryItemFragment> FragmentClass) const;

	/** Returns an equipment fragment of the given type */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Item", meta = (DeterminesOutputType = "FragmentClass"))
	GAMEPLAYINVENTORYSYSTEM_API virtual class UGameplayEquipmentFragment* FindEquipmentFragmentByClass(TSubclassOf<UGameplayEquipmentFragment> FragmentClass) const;

	template <typename T>
	const T* FindItemFragmentByClass() const
	{
		return Cast<T>(FindItemFragmentByClass(T::StaticClass()));
	}

	template <typename T>
	const T* FindEquipmentFragmentByClass() const
	{
		return Cast<T>(FindEquipmentFragmentByClass(T::StaticClass()));
	}

	template <typename T>
	T* GetItemDefinition() const
	{
		return Cast<T>(ItemDefinition);
	}

protected:
	/** The owner component that owns this item instance */
	UPROPERTY(Transient)
	TObjectPtr<class UGameplayInventoryComponent> OwnerComponent;

	/** Called to initialize after being created due to replication */
	GAMEPLAYINVENTORYSYSTEM_API virtual void PostNetInit();

	/** Returns the owning actor of this item instance */
	GAMEPLAYINVENTORYSYSTEM_API virtual AActor* GetOwningActor();

private:
#if UE_WITH_IRIS
	//~ Begin Iris Interface
	GAMEPLAYINVENTORYSYSTEM_API virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
	//~ End Iris Interface
#endif

private:
	/** List of replicated stat tags */
	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;

	/** The item definition associated with this instance */
	UPROPERTY(Replicated)
	TObjectPtr<class UGameplayInventoryItemDefinition> ItemDefinition;

	UPROPERTY(Replicated)
	int32 SlotID;


	mutable FGameplayInventoryItemSpecHandle CurrentSpecHandle;
};

template <typename T>
T* UGameplayInventoryItemInstance::GetOwnerComponent() const
{
	if (OwnerComponent)
	{
		return Cast<T>(OwnerComponent);
	}

	AActor* Owner = Cast<AActor>(GetOuter());
	if (Owner->FindComponentByClass<T>())
	{
		return Owner->FindComponentByClass<T>();
	}

	return nullptr;
}
