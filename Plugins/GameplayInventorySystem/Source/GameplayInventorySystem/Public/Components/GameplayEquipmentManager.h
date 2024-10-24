// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayInventoryComponent.h"
#include "Definitions/GameplayEquipmentDefinition.h"
#include "Definitions/GameplayInventoryItemDefinition.h"
#include "Spec/GameplayEquipmentSpec.h"
#include "GameplayEquipmentManager.generated.h"

struct FGameplayEquipmentSpec;

/**
 * UGameplayEquipmentManager
 *
 * An equipment manager that manages equipment of a player or other entity.
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class GAMEPLAYINVENTORYSYSTEM_API UGameplayEquipmentManager : public UGameplayInventoryComponent
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void BeginPlay() override;
	//~ End UObject Interface

	virtual void OnItemRemoved(const FGameplayInventoryItemSpecHandle& ItemHandle);
	
	virtual void EquipItem(const FGameplayInventoryItemSpecHandle& ItemHandle, const FGameplayInventoryItemContext& ItemContext, UObject* Instigator);
	virtual void UnequipItem(const FGameplayInventoryItemSpecHandle& EquipmentHandle);

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment", meta = (DisplayName = "Equip Item (by Handle)"))
	void EquipItemByHandle(const FGameplayInventoryItemSpecHandle& Handle, const FGameplayInventoryItemContext& ItemContext);

	/** Returns the equipment spec from the handle */
	FGameplayEquipmentSpec* FindEquipmentSpecFromHandle(const FGameplayInventoryItemSpecHandle& Handle) const;

	/** Returns the first equipment instance of the specified type, or nullptr if none exists */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	UGameplayEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UGameplayEquipmentInstance> EquipmentType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return static_cast<T*>(GetFirstInstanceOfType(T::StaticClass()));
	}

public:
	virtual void OnEquipItem(const FGameplayEquipmentSpec& EquipmentSpec, UObject* Instigator);
	virtual void OnUnequipItem(const FGameplayEquipmentSpec& EquipmentSpec, UObject* Instigator);

	virtual void AddReplicatedEquipmentInstance(UGameplayEquipmentInstance* EquipmentInstance);
	virtual void CreateNewInstanceOfEquipment(FGameplayEquipmentSpec& Spec, const FGameplayInventoryItemContext& ItemContext);
	
protected:
	UPROPERTY(ReplicatedUsing = OnRep_EquipmentList, BlueprintReadOnly, Transient, Category = "Equipment")
	FGameplayEquipmentSpecContainer EquipmentList;

	UFUNCTION()
	virtual void OnRep_EquipmentList();
	
private:
	void EquipItem_Internal(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& ItemContext, UGameplayInventoryItemInstance* Instigator);
};
