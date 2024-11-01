// Copyright © 2024 MajorT. All rights reserved.


#include "Components/GameplayInventoryManager.h"

#include "GameplayInventoryLogChannels.h"
#include "Components/GameplayEquipmentManager.h"
#include "Engine/ActorChannel.h"
#include "Instance/GameplayInventoryItemInstance.h"
#include "Definitions/GameplayInventoryItemDefinition.h"
#include "Library/InventorySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Requirements/GameplayInventoryRequirement.h"
#include "Rows/GameplayInventoryRowConfig.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayInventoryManager)

FOnShowDebugInfo UGameplayInventoryManager::OnShowDebugInfoDelegate;

UGameplayInventoryManager::UGameplayInventoryManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
	SetIsReplicatedByDefault(true);
}

void UGameplayInventoryManager::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_OwnerOnly;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, InventoryList, Params);
}

bool UGameplayInventoryManager::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	for (UGameplayInventoryItemInstance* Instance : InventoryList.GetAllInstances())
	{
		if (Instance && IsValid(Instance))
		{
			bWroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}

void UGameplayInventoryManager::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register all the existing item instances
	if (!IsUsingRegisteredSubObjectList())
	{
		return;
	}

	for (UGameplayInventoryItemInstance* Instance : InventoryList.GetAllInstances())
	{
		if (IsValid(Instance))
		{
			AddReplicatedSubObject(Instance);
		}
	}
}

void UGameplayInventoryManager::OnUnregister()
{
	Super::OnUnregister();
}

void UGameplayInventoryManager::BeginPlay()
{
	Super::BeginPlay();

	if (InventoryList.OwnerComponent != nullptr && InventoryList.OwnerComponent != this)
	{
		return;
	}

	const AActor* Owner = GetOwner();
	check(Owner);

	InventoryList.OwnerComponent = this;
}

bool UGameplayInventoryManager::CanAddItemDef(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& Context)
{
	UGameplayInventoryItemDefinition* ItemDef = ItemSpec.Item;
	if (!IsValid(ItemDef))
	{
		LOG_INVENTORY(Error, TEXT("CanAddItemDef called with invalid item definition"));
		return false;
	}

	// Check for blueprint implementation
	if (!K2_CanAddItemDef(ItemSpec, Context))
	{
		return false;
	}


	// Check requirements
	for (UGameplayInventoryRequirement* Requirement : ItemDef->ItemRequirements)
	{
		if (Requirement == nullptr)
		{
			LOG_INVENTORY(Warning, TEXT("Item definition %s has a null requirement"), *ItemDef->GetPathName());
			continue;
		}
		
		if (!Requirement->CanInventoryItemPerformAction(ItemSpec, Context))
		{
			return false;
		}
	}

	// Check for rows
	if (const UGameplayInventoryRowConfig* RowConfig = FindRowConfig(ItemSpec))
	{
		if (!CanAddItemToRow(ItemSpec, Context, RowConfig))
		{
			return false;
		}
	}

	return true;
}

bool UGameplayInventoryManager::K2_CanAddItemDef_Implementation(
	const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& Context)
{
	return true;
}

bool UGameplayInventoryManager::CanAddItemFullyToExisting(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& Context, FGameplayInventoryItemSpec& ExistingSpec)
{
	const int32 StacksToAdd = Context.StackCount;
	
	if (ExistingSpec.GetItemDefinition() == nullptr)
	{
		return false;
	}
	
	const FGameplayInventoryItemStackingData& StackingData = ExistingSpec.GetItemDefinition()->StackingData;
	
	if (!StackingData.bCanStack)
	{
		return false;
	}
	
	if (StackingData.MaxStackSize < StacksToAdd + ExistingSpec.GetStackCount())
	{
		return false;	
	}

	return true;
}

bool UGameplayInventoryManager::CanAddItemToRow(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& Context, const UGameplayInventoryRowConfig* RowConfig)
{
	if (RowConfig == nullptr)
	{
		return true;
	}

	switch (RowConfig->Limits) {
	case EGameplayInventoryLimits::NoLimit:
		{
			return true;
		}
	case EGameplayInventoryLimits::ItemLimit:
		{
			int32 ExistingStackCount = Context.StackCount;
			const TArray<FGameplayInventoryItemSpec> Specs = GetItemSpecsInRow(RowConfig->RowTag);
			for (const auto& Item : Specs)
			{
				ExistingStackCount += Item.GetStackCount();
			}

			if (ExistingStackCount > RowConfig->MaxItemCount)
			{
				return false;
			}
		}
	case EGameplayInventoryLimits::SlotLimit:
		{
			const int32 NumSpecs = GetItemSpecsInRow(RowConfig->RowTag).Num();

			FGameplayInventoryItemSpec Existing;
			if (ItemSpec.GetItemDefinition()->StackingData.bCanStack && CanAddItemFullyToExisting(ItemSpec, Context, Existing))
			{
				return true;
			}

			LOG_INVENTORY(Warning, TEXT("Final size: %d, Max Size: %d"), (NumSpecs + 1), RowConfig->MaxSlotCount);

			if ((NumSpecs + 1) > RowConfig->MaxSlotCount)
			{
				return false;
			}

			return true;
		}
	case EGameplayInventoryLimits::WeightLimit:
		{
			//@TODO: Weight currently not supported
		}
	}
	
	return true;
}

UGameplayInventoryRowConfig* UGameplayInventoryManager::FindRowConfig(const FGameplayInventoryItemSpec& ItemSpec)
{
	UGameplayInventoryRowConfig* RowConfig = nullptr;

	for (UGameplayInventoryRowConfig* Config : RowConfigs)
	{
		if (!Config->DoesSpecMatch(ItemSpec))
		{
			continue;
		}

		RowConfig = Config;
		break;
	}

	return RowConfig;
}

FGameplayInventoryItemSpecHandle UGameplayInventoryManager::GiveItem(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& Context)
{
	if (!IsValid(ItemSpec.Item))
	{
		LOG_INVENTORY(Error, TEXT("GiveItem called with invalid item definition"));
		return FGameplayInventoryItemSpecHandle();
	}

	if (!IsOwnerActorAuthoritative())
	{
		LOG_INVENTORY(Error, TEXT("GiveItem called on client, ignoring"));
		return FGameplayInventoryItemSpecHandle();
	}

	if (!CanAddItemDef(ItemSpec, Context))
	{
		LOG_INVENTORY(Error, TEXT("GiveItem called with item that cannot be added"));
		return FGameplayInventoryItemSpecHandle();
	}

	FGameplayInventoryItemContext NewContext = Context;
	if (!NewContext.IsValid())
	{
		NewContext = FGameplayInventoryItemContext(GetOwner());
	}

	UGameplayInventoryItemDefinition* ItemDef = ItemSpec.Item;
	int32 StacksToAdd = ItemSpec.StackCount;
	int32 ItemSpecIndex = INDEX_NONE;

	// Try to fill existing stacks first
	if (ItemSpec.GetItemDefinition()->StackingData.bCanStack)
	{
		for (FGameplayInventoryItemSpec& Spec : InventoryList.Items)
		{
			// If we have no more stacks left to add, break
			if (StacksToAdd <= 0)
			{
				break;
			}

			// If the item definition doesn't stack, skip
			if (!Spec.Item->StackingData.bCanStack)
			{
				continue;
			}

			// If the item definition doesn't match, skip
			if (Spec.Item != ItemDef)
			{
				continue;
			}

			// If the stack is full, skip
			if (Spec.IsStackFull())
			{
				continue;
			}

			const int32 Delta = FMath::Min(StacksToAdd, Spec.GetMaxStackCount() - Spec.GetStackCount());
			Spec.StackCount += Delta;

			StacksToAdd -= Delta;
			ItemSpecIndex = InventoryList.Items.Find(Spec);

			OnChangeItem(Spec);
			MarkInventoryItemSpecDirty(Spec);
		}	
	}

	// If we still have stacks to add, create new specs
	if (StacksToAdd > 0)
	{
		const int32 Delta = FMath::Min(StacksToAdd, ItemDef->StackingData.MaxStackSize);
		StacksToAdd -= Delta;

		const FGameplayInventoryItemSpec TempSpec(ItemDef, Delta, GetOwner());
		FGameplayInventoryItemSpec& NewSpec = InventoryList.Items.Add_GetRef(TempSpec);

		// Assign the row tag if it exists
		if (auto* Config = FindRowConfig(NewSpec))
		{
			NewSpec.RowTag = Config->RowTag;
		}

		if (ShouldCreateNewInstanceOfItem(ItemSpec))
		{
			CreateNewInstanceOfItem(NewSpec, NewContext);
		}

		ItemSpecIndex = InventoryList.Items.Find(NewSpec);
		
		OnGiveItem(NewSpec);
		MarkInventoryItemSpecDirty(NewSpec);

		// Check if we need to add more stacks, if so, call the function again
		if (StacksToAdd > 0)
		{
			const FGameplayInventoryItemSpec RemainingItemSpec(ItemDef, StacksToAdd, GetOwner());
			return GiveItem(RemainingItemSpec, NewContext);
		}
	}

	if (ItemSpecIndex == INDEX_NONE)
	{
		LOG_INVENTORY(Error, TEXT("Failed to give item (%s)"), *ItemDef->GetPathName());
		return FGameplayInventoryItemSpecHandle();
	}

	return InventoryList.Items[ItemSpecIndex].Handle;
}

void UGameplayInventoryManager::RemoveItem(UGameplayInventoryItemInstance* ItemToRemove, const FGameplayInventoryItemContext& Context)
{
	const FGameplayInventoryItemSpecHandle& ItemHandle = ItemToRemove->GetItemSpecHandle();
	RemoveItem(ItemHandle, Context);
}

void UGameplayInventoryManager::ConsumeItem(const FGameplayInventoryItemContext& Context)
{
	if (Context.ItemDefinition == nullptr)
	{
		LOG_INVENTORY(Error, TEXT("ConsumeItem called with invalid item definition"));
		return;
	}

	if (!IsOwnerActorAuthoritative())
	{
		LOG_INVENTORY(Warning, TEXT("ConsumeItem called on client, ignoring"));
		return;
	}
	
	auto Predicate = [Context] (const FGameplayInventoryItemSpec& Other)
	{
		return Other.Item == Context.ItemDefinition;	
	};

	InternalRemoveItemByPredicate(Predicate, Context);
}

void UGameplayInventoryManager::InternalRemoveItemByPredicate(const TFunctionRef<bool(const FGameplayInventoryItemSpec& Other)>& Predicate, const FGameplayInventoryItemContext& Context)
{
	int32 StacksToRemove = GetStackCount(0, Context);

	// Try to remove items that match the predicate
	for (auto It = InventoryList.Items.CreateIterator(); It; ++It)
	{
		// If we have no more stacks left to remove, break
		if (StacksToRemove <= 0)
		{
			break;
		}
		
		FGameplayInventoryItemSpec& Spec = *It;
		check(Spec.IsValid());

		if (!Predicate(Spec))
		{
			LOG_INVENTORY(Display, TEXT("Item %s: Predicate did not match"), *Spec.Item->GetPathName());
			continue;
		}
		
		const int32 Delta = FMath::Min(StacksToRemove, Spec.StackCount);
		Spec.StackCount -= StacksToRemove;
		StacksToRemove -= Delta;

		LOG_INVENTORY(Display, TEXT("Item %s Handle: %s: Removed %d stacks"), *Spec.Item->GetPathName(), *Spec.GetHandle().ToString(), Delta);

		MarkInventoryItemSpecDirty(Spec);

		// Remove the item if the stack count is 0
		if (Spec.StackCount <= 0)
		{
			// Temporarily store the spec so we can call OnRemoveItem
			FGameplayInventoryItemSpec Temp = Spec;
			OnRemoveItem(Temp);
			
			It.RemoveCurrent();
			InventoryList.MarkArrayDirty();
		}
	}
}

void UGameplayInventoryManager::RemoveItem(const FGameplayInventoryItemSpecHandle& ItemToRemoveHandle, const FGameplayInventoryItemContext& Context)
{
	if (!ItemToRemoveHandle.IsValid())
	{
		LOG_INVENTORY(Error, TEXT("RemoveItem called with invalid item handle"));
		return;
	}

	if (!IsOwnerActorAuthoritative())
	{
		LOG_INVENTORY(Error, TEXT("RemoveItem called on client, ignoring"));
		return;
	}

	auto Predicate = [ItemToRemoveHandle] (const FGameplayInventoryItemSpec& Other)
	{
		return Other.Handle == ItemToRemoveHandle;
	};

	InternalRemoveItemByPredicate(Predicate, Context);
}

/*FGameplayInventoryItemSpecHandle UGameplayInventoryManager::K2_GiveItemFromSpec(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& Context)
{
	// Check if we have a valid context
	FGameplayInventoryItemContext NewContext = Context;
	if (!NewContext.IsValid())
	{
		NewContext = FGameplayInventoryItemContext(GetOwner());
		NewContext.StackCount = ItemSpec.StackCount;
	}

	// Check if we have a valid item spec
	if (!ItemSpec.IsValid())
	{
		LOG_INVENTORY(Error, TEXT("K2_GiveItem called with invalid item spec"));
		return FGameplayInventoryItemSpecHandle();
	}

	// Give the item and return the handle
	return GiveItem(ItemSpec, NewContext);
}*/

FGameplayInventoryItemSpecHandle UGameplayInventoryManager::K2_GiveItemFromContext(const FGameplayInventoryItemContext& Context)
{
	check(Context.ItemDefinition);

	const FGameplayInventoryItemSpec NewSpec(Context.ItemDefinition, FMath::Max(1, Context.StackCount), Context.Instigator);
	return GiveItem(NewSpec, Context);
}

/*FGameplayInventoryItemSpecHandle UGameplayInventoryManager::K2_GiveItemFromDefinition( UGameplayInventoryItemDefinition* ItemDefinition, const FGameplayInventoryItemContext& Context, int32 InStackCount)
{
	// Check if we have a valid context
	FGameplayInventoryItemContext NewContext = Context;
	if (!NewContext.IsValid())
	{
		NewContext = FGameplayInventoryItemContext(GetOwner());
	}

	if (NewContext.StackCount <= 0)
	{
		NewContext.StackCount = InStackCount;
	}

	// Construct a new item spec
	const FGameplayInventoryItemSpec NewSpec = ConstructInventoryItemSpecFromDefinition(ItemDefinition, NewContext);

	// Give the item and return the handle
	return GiveItem(NewSpec, NewContext);
}*/

void UGameplayInventoryManager::K2_RemoveItem(const FGameplayInventoryItemSpecHandle& ItemToRemoveHandle, const FGameplayInventoryItemContext& Context)
{
	FGameplayInventoryItemContext NewContext = Context;
	if (!NewContext.IsValid())
	{
		NewContext = FGameplayInventoryItemContext(GetOwner());
	}

	RemoveItem(ItemToRemoveHandle, NewContext);
}

void UGameplayInventoryManager::K2_RemoveItemByInstance(UGameplayInventoryItemInstance* ItemToRemove, const FGameplayInventoryItemContext& Context)
{
	FGameplayInventoryItemContext NewContext = Context;
	if (!NewContext.IsValid())
	{
		NewContext = FGameplayInventoryItemContext(GetOwner());
	}

	RemoveItem(ItemToRemove, NewContext);
}

void UGameplayInventoryManager::K2_ConsumeItem(const FGameplayInventoryItemContext& Context)
{
	FGameplayInventoryItemContext NewContext = Context;
	if (!NewContext.IsValid())
	{
		NewContext = FGameplayInventoryItemContext(GetOwner());
	}

	ConsumeItem(NewContext);
}

TArray<FGameplayInventoryItemSpecHandle> UGameplayInventoryManager::GetInventoryList() const
{
	TArray<FGameplayInventoryItemSpecHandle> Handles;
	Handles.Reserve(InventoryList.Items.Num());
	for (const FGameplayInventoryItemSpec& Spec : InventoryList.Items)
	{
		Handles.Add(Spec.Handle);
	}
	return Handles;
}

TArray<FGameplayInventoryItemSpec>& UGameplayInventoryManager::GetItemSpecs()
{
	return InventoryList.Items;
}

const TArray<FGameplayInventoryItemSpec>& UGameplayInventoryManager::GetItemSpecs() const
{
	return InventoryList.Items;
}

const TArray<FGameplayInventoryItemSpec> UGameplayInventoryManager::GetItemSpecsInRow(const FGameplayTag& RowTag) const
{
	TArray<FGameplayInventoryItemSpec> Items;
	
	for (const auto& Spec : InventoryList.Items)
	{
		if (!Spec.RowTag.IsValid())
		{
			continue;
		}

		if (Spec.RowTag.MatchesTagExact(RowTag))
		{
			Items.Add(Spec);
		}
	}

	return Items;
}

int32 UGameplayInventoryManager::GetTotalItemCountByDefinition(UGameplayInventoryItemDefinition* InItemDef) const
{
	int32 TotalCount = 0;

	for (const auto& Item : InventoryList.Items)
	{
		if (!Item.IsValid())
		{
			continue;
		}

		if (Item.GetItemDefinition() != InItemDef)
		{
			continue;
		}

		TotalCount += Item.GetStackCount();
	}

	return TotalCount;
}

const TArray<UGameplayInventoryRowConfig*>& UGameplayInventoryManager::GetRowConfig() const
{
	return RowConfigs;
}

FGameplayInventoryItemContext UGameplayInventoryManager::MakeItemContext(UGameplayInventoryItemDefinition* ItemDefinition, const int32 StackCount, UObject* Instigator, const FGameplayTagContainer ContextTags)
{
	FGameplayInventoryItemContext New(GetOwner());
	New.ItemDefinition = ItemDefinition;
	New.StackCount = StackCount;
	New.ContextTags = ContextTags;
	New.Instigator = Instigator;

	if (New.Instigator == nullptr)
	{
		New.Instigator = GetOwner();
	}

	if (New.InventoryComponent == nullptr)
	{
		New.InventoryComponent = this;
	}

	return New;
}

FGameplayInventoryItemContext UGameplayInventoryManager::FindItemContextFromHandle(
	const FGameplayInventoryItemSpecHandle& ItemHandle) const
{
	if (!ItemHandle.IsValid())
	{
		LOG_INVENTORY(Verbose, TEXT("FindItemContextFromHandle called with invalid item handle"));
		return FGameplayInventoryItemContext();
	}
	
	const FGameplayInventoryItemSpec& ItemSpec = *FindItemSpecFromHandle(ItemHandle);
	if (ItemSpec.IsValid())
	{
		return ItemSpec.GetItemContext();
	}

	return FGameplayInventoryItemContext();
}

FGameplayInventoryItemSpec* UGameplayInventoryManager::FindItemSpecFromHandle(const FGameplayInventoryItemSpecHandle& ItemHandle) const
{
	for (const FGameplayInventoryItemSpec& Spec : InventoryList.Items)
	{
		if (Spec.Handle != ItemHandle)
		{
			continue;
		}

		return const_cast<FGameplayInventoryItemSpec*>(&Spec);
	}

	return nullptr;
}

FGameplayInventoryItemSpec* UGameplayInventoryManager::FindItemSpecFromDefinition(UGameplayInventoryItemDefinition* ItemDefinition) const
{
	for (const FGameplayInventoryItemSpec& Spec : InventoryList.Items)
	{
		if (Spec.Item != ItemDefinition)
		{
			continue;
		}

		return const_cast<FGameplayInventoryItemSpec*>(&Spec);
	}

	return nullptr;
}

FGameplayInventoryItemSpec UGameplayInventoryManager::ConstructInventoryItemSpecFromDefinition( UGameplayInventoryItemDefinition* InItemDef, const FGameplayInventoryItemContext& InContext)
{
	if (!ensure(InItemDef))
	{
		LOG_INVENTORY(Error, TEXT("ConstructInventoryItemSpecFromDefinition called with invalid item definition"));
		return FGameplayInventoryItemSpec();
	}

	return FGameplayInventoryItemSpec(InItemDef, InContext.StackCount, InContext.Instigator);
}

void UGameplayInventoryManager::MarkInventoryItemSpecDirty(FGameplayInventoryItemSpec& InSpec)
{
	if (IsOwnerActorAuthoritative())
	{
		if (InSpec.GetInstance())
		{
			InventoryList.MarkItemDirty(InSpec);
		}
	}
	else
	{
		InventoryList.MarkArrayDirty();
	}
}

UGameplayInventoryItemInstance* UGameplayInventoryManager::CreateNewInstanceOfItem( FGameplayInventoryItemSpec& InItemSpec, const FGameplayInventoryItemContext& InContext)
{
	UGameplayInventoryItemDefinition* ItemDef = InItemSpec.Item;
	check(ItemDef);

	AActor* Owner = GetOwner();
	check(Owner);

	UGameplayInventoryItemInstance* NewInstance = NewObject<UGameplayInventoryItemInstance>(Owner, ItemDef->GetInstanceClass());
	check(NewInstance);
	InItemSpec.Instance = NewInstance;
	NewInstance->SetItemDef(ItemDef);

	// Notify the item instance that it has been created
	// NewInstance->OnInstanceCreated(InItemSpec.Handle, &InContext);

	// Add the instance to the replicated sub-object list
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		constexpr ELifetimeCondition Condition = COND_ReplayOrOwner;
		AddReplicatedSubObject(NewInstance, Condition);
	}

	return NewInstance;
}

bool UGameplayInventoryManager::ShouldCreateNewInstanceOfItem(const FGameplayInventoryItemSpec& InItemSpec)
{
	UGameplayInventoryItemDefinition* ItemDef = InItemSpec.Item;
	check(ItemDef);

	// If the item doesn't stack, we need to create a new instance
	if (!ItemDef->StackingData.bCanStack)
	{
		return true;
	}

	bool bResult = true;
	const FGameplayInventoryItemSpec* FoundSpec = FindItemSpecFromDefinition(ItemDef);
	if (FoundSpec == nullptr)
	{
		return true;
	}

	if (FoundSpec->IsStackFull())
	{
		bResult = true;
	}

	return bResult;
}

void UGameplayInventoryManager::OnGiveItem(FGameplayInventoryItemSpec& InSpec)
{
	if (!InSpec.GetInstance())
	{
		return;
	}

	// Notify the item instance that it has been given
	UGameplayInventoryItemInstance* Instance = InSpec.GetInstance();
	if (Instance != nullptr)
	{
		Instance->OnInstanceCreated(InSpec.Handle, nullptr);
	}

	const APawn* OwnerPawn = GetOwnerPawn();
	if (OwnerPawn && OwnerPawn->IsLocallyControlled() && OwnerPawn->HasAuthority())
	{
		InventoryList.BroadcastInventoryChangeMessage(Instance, 0, InSpec.StackCount);
	}

	K2_OnGiveItem(InSpec);
}

void UGameplayInventoryManager::OnRemoveItem(FGameplayInventoryItemSpec& InSpec)
{
	if (!InSpec.GetInstance())
	{
		return;
	}

	OnItemRemoved.Broadcast(InSpec.Handle);

	LOG_INVENTORY(Display, TEXT("OnRemoveItem: Item %s Handle: %s"), *InSpec.Item->GetPathName(), *InSpec.Handle.ToString());

	K2_OnRemoveItem(InSpec);

	// Notify the equipment manager that the item has been removed
	if (UGameplayEquipmentManager* EquipmentManager = UInventorySystemBlueprintLibrary::FindEquipmentManager(GetOwner()))
	{
		EquipmentManager->OnItemRemoved(InSpec.Handle);
	}
	else
	{
		LOG_INVENTORY(Warning, TEXT("OnRemoveItem: Owner: %s does not have an equipment manager"), *GetOwner()->GetName());
	}

	// Notify the item instance that it has been removed
	FGameplayInventoryItemContext Context(GetOwner());
	Context.InventoryComponent = this;
	Context.ItemDefinition = InSpec.Item;
	Context.ItemInstance = InSpec.GetInstance();
	
	UGameplayInventoryItemInstance* Instance = InSpec.GetInstance();
	if (ensure(Instance))
	{
		Instance->OnInstanceRemoved(InSpec.Handle, &Context);

		if (GetOwnerRole() == ROLE_Authority)
		{
			Instance->MarkAsGarbage();
		}
	}

	const APawn* OwnerPawn = GetOwnerPawn();
	if (OwnerPawn && OwnerPawn->IsLocallyControlled() && OwnerPawn->HasAuthority())
	{
		InventoryList.BroadcastInventoryChangeMessage(Instance, InSpec.StackCount, 0);
	}
}

void UGameplayInventoryManager::OnChangeItem(FGameplayInventoryItemSpec& InSpec)
{
	if (!InSpec.GetInstance())
	{
		return;
	}

	// Notify the item instance that it has been changed
	UGameplayInventoryItemInstance* Instance = InSpec.GetInstance();
	if (Instance != nullptr)
	{
		Instance->OnInstanceChanged(InSpec.Handle /*@TODO: Context */);
	}

	const APawn* OwnerPawn = GetOwnerPawn();
	if (OwnerPawn && OwnerPawn->IsLocallyControlled() && OwnerPawn->HasAuthority())
	{
		InventoryList.BroadcastInventoryChangeMessage(Instance, InSpec.LastObservedStackCount, InSpec.StackCount);
	}
}

void UGameplayInventoryManager::OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo,float& YL, float& YPos)
{
	OnShowDebugInfoDelegate.Broadcast(HUD, Canvas, DisplayInfo, YL, YPos);
}

int32 UGameplayInventoryManager::GetStackCount(const int32& StackCount, const FGameplayInventoryItemContext& Context) const
{
	int32 OutStackCount = INT_MAX;
	
	if (Context.StackCount > 0)
	{
		OutStackCount = Context.StackCount;
	}

	if (StackCount > 0)
	{
		OutStackCount = StackCount;
	}
	
	return OutStackCount;
}

void UGameplayInventoryManager::BroadcastInventoryChangeMessage(UGameplayInventoryItemInstance* ItemInstance, int32 OldStackCount, int32 NewStackCount)
{
	InventoryList.BroadcastInventoryChangeMessage(ItemInstance, OldStackCount, NewStackCount);
}

void UGameplayInventoryManager::OnRegister()
{
	Super::OnRegister();
}

void UGameplayInventoryManager::InitializeComponent()
{
	Super::InitializeComponent();

	if (InventoryList.OwnerComponent != nullptr && InventoryList.OwnerComponent != this)
	{
		return;
	}

	const AActor* Owner = GetOwner();
	check(Owner);

	InventoryList.OwnerComponent = this;
}

void UGameplayInventoryManager::OnRep_InventoryList()
{
	// Validate that all item instances have replicated
	for (FGameplayInventoryItemSpec& Spec : InventoryList.Items)
	{
		const UGameplayInventoryItemInstance* Instance = Spec.Instance;
		if (!Instance)
		{
			// Queue up another call to make sure this gets run again, as our item instance may not have replicated yet
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_InventoryList, this, &UGameplayInventoryManager::OnRep_InventoryList, 0.5f, false);
			return;
		}
	}

	for (UGameplayInventoryItemInstance* Instance : InventoryList.GetAllInstances())
	{
		if (Instance->GetOwnerComponent() != nullptr)
		{
			continue;
		}

		Instance->PostNetInit();
	}
}
