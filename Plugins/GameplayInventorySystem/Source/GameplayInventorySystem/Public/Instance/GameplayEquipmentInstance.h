// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayInventoryTypes.h"
#include "Definitions/GameplayEquipmentDefinition.h"
#include "Spec/GameplayEquipmentSpec.h"
#include "UObject/Object.h"
#include "GameplayEquipmentInstance.generated.h"

struct FGameplayEquipmentActorToSpawn;
struct FGameplayEquipmentSpec;

 /**
 * UGameplayEquipmentInstance
 *
 * A replicated instance of applied equipment in the inventory system.
 */
UCLASS(BlueprintType, Blueprintable)
class GAMEPLAYINVENTORYSYSTEM_API UGameplayEquipmentInstance : public UObject
{
	GENERATED_UCLASS_BODY()
	friend class UGameplayEquipmentManager;

public:
	//~ Begin UObject Interface
	virtual UWorld* GetWorld() const override;
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack) override;
	//~ End UObject Interface

	/** Returns the current item context */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	const FGameplayInventoryItemContext& GetCurrentItemContext() const { return CurrentContext; }

	/** Returns the instigator that applied this equipment */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	UObject* GetInstigator() const { return Instigator; }

	/** Returns the owning pawn */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	APawn* GetPawn() const;

	/** Returns the owning pawn (typed) */
	UFUNCTION(BlueprintPure, Category = "Equipment", meta = (DeterminesOutputType = PawnType))
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnClass) const;

	/** Returns the item definition */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	UGameplayInventoryItemDefinition* GetItemDefinition() const;

	/** Returns the equipment definition */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	UGameplayEquipmentDefinition* GetEquipmentDefinition() const;

	/** Returns the spawned equipment actors */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	const TArray<AActor*>& GetSpawnedEquipmentActors() const { return SpawnedEquipmentActors; }

	/** Returns the first spawned equipment actor */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	AActor* GetSpawnedEquipmentActor() const { return SpawnedEquipmentActors.Num() > 0 ? SpawnedEquipmentActors[0] : nullptr; }

public:
	virtual void OnEquipped(const FGameplayEquipmentSpec& EquipmentSpec, const FGameplayInventoryItemContext& Context);
	virtual void OnUnequipped(const FGameplayEquipmentSpec& EquipmentSpec);

	virtual void SpawnEquipmentActors(const TArray<FGameplayEquipmentActorSpawnQuery>& Queries);
	virtual void DestroyEquipmentActors();


	template <typename T>
	T* GetItemDefinition() const
	{
		return Cast<T>(GetItemDefinition());
	}

protected:
	/**
	 * Called when the equipment has been equipped by the owner
	 * @param EquipmentSpec			The equipment spec that was applied
	 * @param Handle				The handle of the item that was equipped
	 * @param Context				The context of how the item was equipped
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", meta = (DisplayName = "OnEquipped"))
	void K2_OnEquipped(const FGameplayEquipmentSpec& EquipmentSpec, const FGameplayInventoryItemSpecHandle& Handle, const FGameplayInventoryItemContext& Context);

	/**
	 * Called when the owner has unequipped the equipment
	 * @param EquipmentSpec		The equipment spec that was removed
	 * @param Handle			The handle of the item that was unequipped
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", meta = (DisplayName = "OnUnequipped"))
	void K2_OnUnequipped(const FGameplayEquipmentSpec& EquipmentSpec, const FGameplayInventoryItemSpecHandle& Handle);

protected:
#if UE_WITH_IRIS
	/** Register all replication fragments */
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif

protected:
	UFUNCTION()
	virtual void OnRep_Instigator();
	
protected:
	/** The instigator that applied this equipment */
	UPROPERTY(ReplicatedUsing = OnRep_Instigator)
	TObjectPtr<UObject> Instigator;
	
	/** A list of all actors that were spawned by this equipment */
	UPROPERTY(BlueprintReadWrite, Category = "Equipment")
	TArray<TObjectPtr<AActor>> SpawnedEquipmentActors;

	/** The context on how this	equipment was applied */
	UPROPERTY(Replicated)
	FGameplayInventoryItemContext CurrentContext;
};
