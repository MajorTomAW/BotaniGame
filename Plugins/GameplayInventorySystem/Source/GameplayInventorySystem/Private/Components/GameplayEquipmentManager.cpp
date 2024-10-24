// Copyright © 2024 MajorT. All rights reserved.


#include "Components/GameplayEquipmentManager.h"

#include "GameplayInventoryLogChannels.h"
#include "Instance/GameplayEquipmentInstance.h"
#include "Instance/GameplayInventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif

#include "Engine/ActorChannel.h"
#include "Fragments/GameplayEquipmentFragment.h"
#include "Fragments/GameplayInventoryItemFragment.h"
#include "Library/InventorySystemBlueprintLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayEquipmentManager)

UGameplayEquipmentManager::UGameplayEquipmentManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
}

void UGameplayEquipmentManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_None;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, EquipmentList, Params);
}

bool UGameplayEquipmentManager::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch,FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	for (FGameplayEquipmentSpec& Spec : EquipmentList.Items)
	{
		UGameplayEquipmentInstance* EquipmentInstance = Spec.Instance;

		if (IsValid(EquipmentInstance))
		{
			bWroteSomething |= Channel->ReplicateSubobject(EquipmentInstance, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}

void UGameplayEquipmentManager::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (FGameplayEquipmentSpec& Spec : EquipmentList.Items)
		{
			UGameplayEquipmentInstance* EquipmentInstance = Spec.Instance;

			if (IsValid(EquipmentInstance))
			{
				AddReplicatedSubObject(EquipmentInstance);
			}
		}
	}
}

void UGameplayEquipmentManager::OnRegister()
{
	Super::OnRegister();

	APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (OwningPawn)
	{
		return;
	}

	LOG_INVENTORY(Error, TEXT("GameplayEquipmentManager on [%s] can only be added to pawn actors."), *GetNameSafe(GetOwner()));

#if WITH_EDITOR
	if (GIsEditor)
	{
		static const FText Message = NSLOCTEXT("GameplayEquipmentManager", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST ne placed on a Pawn Blueprint. This will cause a crash in PIE!");
		static const FName Title = TEXT("GameplayEquipmentManager");

		FMessageLog(Title).Error()
		->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
		->AddToken(FTextToken::Create(Message));

		FMessageLog(Title).Open();
	}
#endif
}

void UGameplayEquipmentManager::OnUnregister()
{
	Super::OnUnregister();
}

void UGameplayEquipmentManager::BeginPlay()
{
	Super::BeginPlay();
}

void UGameplayEquipmentManager::OnItemRemoved(const FGameplayInventoryItemSpecHandle& ItemHandle)
{
	UnequipItem(ItemHandle);
}

void UGameplayEquipmentManager::EquipItem(const FGameplayInventoryItemSpecHandle& ItemHandle, const FGameplayInventoryItemContext& ItemContext, UObject* Instigator)
{
	check(Instigator);

	UGameplayInventoryItemInstance* ItemInstance = Cast<UGameplayInventoryItemInstance>(Instigator);
	if (ItemInstance == nullptr)
	{
		LOG_INVENTORY(Error, TEXT("Failed to equip item. Instigator is not an item instance."));
		return;
	}

	FGameplayInventoryItemContext ContextCopy = ItemContext;
	ContextCopy.ItemDefinition = ItemInstance->GetItemDefinition();
	
	if (ContextCopy.ItemDefinition == nullptr)
	{
		LOG_INVENTORY(Error, TEXT("Failed to equip item. Item definition is null."));
		return;
	}

	// Make sure the item is not already equipped
	if (EquipmentList.FindSpecByHandle(ItemHandle) != nullptr)
	{
		LOG_INVENTORY(Verbose, TEXT("Item is already equipped. Skipping equip."));
		return;
	}
	
	const FGameplayInventoryItemSpec& ItemSpec = *ItemInstance->GetOwnerInventoryManager()->FindItemSpecFromHandle(ItemHandle);
	
	EquipItem_Internal(ItemSpec, ContextCopy, ItemInstance);
}

void UGameplayEquipmentManager::UnequipItem(const FGameplayInventoryItemSpecHandle& EquipmentHandle)
{
	FGameplayEquipmentSpec* EquipmentSpec = FindEquipmentSpecFromHandle(EquipmentHandle);
	if (EquipmentSpec && EquipmentSpec->IsValid())
	{
		for (auto It = EquipmentList.Items.CreateIterator(); It; ++It)
		{
			if (It->Handle == EquipmentHandle)
			{
				OnUnequipItem(*EquipmentSpec, GetOwner());
				
				It.RemoveCurrent();
				
				LOG_INVENTORY(Display, TEXT("Unequipped item with handle: %s"), *EquipmentHandle.ToString());
				break;
			}
		}
		
		EquipmentList.MarkArrayDirty();
	}
	else
	{
		LOG_INVENTORY(Warning, TEXT("Failed to unequip item. Equipment spec is invalid."));
	}
}

void UGameplayEquipmentManager::EquipItemByHandle(const FGameplayInventoryItemSpecHandle& Handle, const FGameplayInventoryItemContext& ItemContext)
{
	UGameplayInventoryItemInstance* ItemInstance = ItemContext.ItemInstance;
	check(ItemInstance);

	FGameplayInventoryItemContext Context = ItemContext;
	if (!Context.InventoryOwner)
	{
		Context.InventoryOwner = GetOwner();
	}

	EquipItem(Handle, Context, ItemInstance);
}

FGameplayEquipmentSpec* UGameplayEquipmentManager::FindEquipmentSpecFromHandle(const FGameplayInventoryItemSpecHandle& Handle) const
{
	for (const FGameplayEquipmentSpec& Spec : EquipmentList.Items)
	{
		if (Spec.Handle != Handle)
		{
			continue;
		}

		return const_cast<FGameplayEquipmentSpec*>(&Spec);
	}

	return nullptr;
}

UGameplayEquipmentInstance* UGameplayEquipmentManager::GetFirstInstanceOfType(TSubclassOf<UGameplayEquipmentInstance> EquipmentType) const
{
	for (auto& Spec : EquipmentList.Items)
	{
		if (!Spec.IsValid())
		{
			continue;
		}

		if (!Spec.Instance || !Spec.Instance->IsA(EquipmentType))
		{
			continue;
		}

		return Spec.Instance;
	}

	return nullptr;
}

void UGameplayEquipmentManager::OnEquipItem(const FGameplayEquipmentSpec& EquipmentSpec, UObject* Instigator)
{
	UGameplayEquipmentInstance* EquipmentInstance = EquipmentSpec.Instance;
	if (IsValid(EquipmentInstance))
	{
		FGameplayInventoryItemContext ItemContext;
		ItemContext.FillContext();
		ItemContext.Instigator = Instigator;
		ItemContext.InventoryOwner = GetOwner();
		ItemContext.ItemInstance = EquipmentSpec.OwningItemSpec.GetInstance();
		ItemContext.StackCount = EquipmentSpec.OwningItemSpec.GetStackCount();
		ItemContext.InventoryComponent = UInventorySystemBlueprintLibrary::FindInventoryManager(GetOwner());
		
		// EquipmentInstance->OnEquipped(EquipmentSpec, ItemContext);
	}
	else
	{
		LOG_INVENTORY(Warning, TEXT("Failed to equip item. Equipment instance is null."));
	}
}

void UGameplayEquipmentManager::OnUnequipItem(const FGameplayEquipmentSpec& EquipmentSpec, UObject* Instigator)
{
	UGameplayEquipmentInstance* EquipmentInstance = EquipmentSpec.Instance;
	if (IsValid(EquipmentInstance))
	{
		EquipmentInstance->OnUnequipped(EquipmentSpec);
	}
	else
	{
		LOG_INVENTORY(Warning, TEXT("Failed to unequip item. Equipment instance is null."));
	}
}

void UGameplayEquipmentManager::AddReplicatedEquipmentInstance(UGameplayEquipmentInstance* EquipmentInstance)
{
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(EquipmentInstance, COND_None);
	}
}

void UGameplayEquipmentManager::CreateNewInstanceOfEquipment(FGameplayEquipmentSpec& Spec, const FGameplayInventoryItemContext& ItemContext)
{
	const UGameplayInventoryManager* InventoryManager = UInventorySystemBlueprintLibrary::FindInventoryManager(Cast<AActor>(GetOwner()));
	check(InventoryManager);

	FGameplayInventoryItemSpec* ItemSpec = InventoryManager->FindItemSpecFromHandle(Spec.Handle);
	
	UGameplayEquipmentDefinition* EquipmentDef = Spec.EquipmentDefinition;
	check(EquipmentDef);

	AActor* Owner = GetOwner();
	check(Owner);

	UGameplayEquipmentInstance* NewInstance = NewObject<UGameplayEquipmentInstance>(Owner, EquipmentDef->GetInstanceClass()); //@TODO: use the item instance as outer?
	NewInstance->Instigator = ItemSpec->GetInstance(); 
	check(NewInstance);
	Spec.Instance = NewInstance;

	// Notify the equipment instance that it has been equipped
	NewInstance->OnEquipped(Spec, ItemContext);

	// Notify the fragments @TODO: Seperate item fragments and equipment fragments ?
	for (UGameplayEquipmentFragment* Fragment : EquipmentDef->EquipmentFragments)
	{
		if (Fragment == nullptr)
		{
			continue;
		}

		// Fragment->OnInstanceCreated()
	}

	AddReplicatedEquipmentInstance(NewInstance);
}

void UGameplayEquipmentManager::OnRep_EquipmentList()
{
}

void UGameplayEquipmentManager::EquipItem_Internal(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& ItemContext, UGameplayInventoryItemInstance* ItemInstance)
{
	const FGameplayEquipmentSpec LocalEquipmentSpec(ItemSpec);
	FGameplayEquipmentSpec& EquipmentSpec = EquipmentList.Items[EquipmentList.Items.Add(LocalEquipmentSpec)];
	
	EquipmentSpec.Handle = ItemInstance->GetItemSpecHandle();
	EquipmentSpec.EquipmentDefinition = ItemContext.ItemDefinition->EquipmentDefinition;

	CreateNewInstanceOfEquipment(EquipmentSpec, ItemContext);

	OnEquipItem(EquipmentSpec, ItemInstance);
	EquipmentList.MarkItemDirty(EquipmentSpec);
}
