// Copyright © 2024 MajorT. All rights reserved.


#include "Instance/GameplayInventoryItemInstance.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif

#include "Net/UnrealNetwork.h"
#include "Components/GameplayInventoryManager.h"
#include "Definitions/GameplayInventoryItemDefinition.h"
#include "Fragments/GameplayInventoryItemFragment.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayInventoryItemInstance)

UGameplayInventoryItemInstance::UGameplayInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, StatTags(this)
	, SlotID(INDEX_NONE)
{
}

void UGameplayInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	if (const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
	{
		BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
	}

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_ReplayOrOwner;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, StatTags, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ItemDefinition, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, SlotID, Params);
}

#if UE_WITH_IRIS
void UGameplayInventoryItemInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;

	// Build descriptors and allocate PropertyReplicationFragments for this object
	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif

UWorld* UGameplayInventoryItemInstance::GetWorld() const
{
	if (const UObject* Outer = GetOuter())
	{
		return Outer->GetWorld();
	}

	return nullptr;
}

bool UGameplayInventoryItemInstance::CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack)
{
	check(!HasAnyFlags(RF_ClassDefaultObject));
	check(GetOuter() != nullptr);

	AActor* Owner = CastChecked<AActor>(GetOuter());

	bool bProcessed = false;

	FWorldContext* const Context = GEngine->GetWorldContextFromWorld(GetWorld());
	if (Context != nullptr)
	{
		for (FNamedNetDriver& Driver : Context->ActiveNetDrivers)
		{
			if (Driver.NetDriver != nullptr && Driver.NetDriver->ShouldReplicateFunction(Owner, Function))
			{
				Driver.NetDriver->ProcessRemoteFunction(Owner, Function, Parms, OutParms, Stack, this);
				bProcessed = true;
			}
		}
	}

	return bProcessed;
}

int32 UGameplayInventoryItemInstance::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	if (HasAnyFlags(RF_ClassDefaultObject) || !IsSupportedForNetworking())
	{
		// This handles absorbing authority/cosmetic
		return GEngine->GetGlobalFunctionCallspace(Function, this, Stack);
	}
	check(GetOuter() != nullptr);
	return GetOuter()->GetFunctionCallspace(Function, Stack);
}

const FGameplayTagStackContainer& UGameplayInventoryItemInstance::GetOwnedGameplayTagStacks() const
{
	return StatTags;
}

void UGameplayInventoryItemInstance::SetItemDef(UGameplayInventoryItemDefinition* InItemDef)
{
	check(InItemDef);
	ItemDefinition = InItemDef;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ItemDefinition, this);
}

void UGameplayInventoryItemInstance::OnInstanceCreated(const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext* InItemContext)
{
	CurrentSpecHandle = InHandle;
	const FGameplayInventoryItemSpec& Spec = *GetOwnerComponent()->FindItemSpecFromHandle(InHandle);

	const FGameplayInventoryItemContext ItemContext = InItemContext ? *InItemContext : FGameplayInventoryItemContext(GetOwnerActor());

	FGameplayTagContainer BlockingTags;
	
	// Notify the fragments that the instance has been created
	for (const UGameplayInventoryItemFragment* Fragment : ItemDefinition->ItemFragments)
	{
		if (Fragment == nullptr)
		{
			continue;
		}

		/*if (!Fragment->CanApplyItemFragment(ItemContext, Spec, &BlockingTags))
		{
			continue;
		}*/

		Fragment->OnItemInstanceCreated(ItemContext, Spec, this);
	}

	K2_OnInstanceCreated(InHandle, ItemContext);
}

void UGameplayInventoryItemInstance::K2_OnInstanceCreated_Implementation( const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext& InItemContext)
{
}

void UGameplayInventoryItemInstance::OnInstanceRemoved(const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext* InItemContext)
{
	const FGameplayInventoryItemSpec& Spec = *GetOwnerComponent()->FindItemSpecFromHandle(InHandle);
	FGameplayTagContainer BlockingTags;

	// Notify the fragments that the instance has been removed
	for (const UGameplayInventoryItemFragment* Fragment : ItemDefinition->ItemFragments)
	{
		if (Fragment == nullptr)
		{
			continue;
		}

		if (!Fragment->CanApplyItemFragment(*InItemContext, Spec, &BlockingTags))
		{
			continue;
		}

		Fragment->OnItemInstanceDestroyed(*InItemContext, Spec, this);
	}

	K2_OnInstanceRemoved(InHandle, *InItemContext);
}

void UGameplayInventoryItemInstance::K2_OnInstanceRemoved_Implementation(const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext& InItemContext)
{
}

void UGameplayInventoryItemInstance::OnInstanceChanged(const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext* InItemContext)
{
}

int32 UGameplayInventoryItemInstance::GetCurrentStackCount() const
{
	const FGameplayInventoryItemSpec& Spec = *GetOwnerComponent()->FindItemSpecFromHandle(CurrentSpecHandle);

	if (!Spec.IsValid())
	{
		return -1;
	}

	return Spec.GetStackCount();
}

FText UGameplayInventoryItemInstance::GetDisplayName() const
{
	return ItemDefinition ? ItemDefinition->GetDisplayName() : FText::GetEmpty();
}

UTexture2D* UGameplayInventoryItemInstance::GetIcon(const bool bUseLargeIfNotFound) const
{
	return ItemDefinition ? ItemDefinition->GetIcon(bUseLargeIfNotFound) : nullptr;
}

FGameplayTagContainer UGameplayInventoryItemInstance::GetConstGameplayTags() const
{
	return ItemDefinition ? ItemDefinition->GetGameplayTags() : FGameplayTagContainer::EmptyContainer;
}


UGameplayInventoryItemDefinition* UGameplayInventoryItemInstance::GetItemDefinitionByType(
	TSubclassOf<UGameplayInventoryItemDefinition> ItemDefinitionType) const
{
	if (ItemDefinition == nullptr)
	{
		return nullptr;
	}

	if (!ItemDefinition->IsA(ItemDefinitionType))
	{
		return nullptr;
	}

	return ItemDefinition;
}

UGameplayInventoryItemFragment* UGameplayInventoryItemInstance::FindItemFragmentByClass(TSubclassOf<UGameplayInventoryItemFragment> FragmentClass) const
{
	if ((ItemDefinition != nullptr) && (FragmentClass != nullptr))
	{
		return ItemDefinition->GetItemFragment(FragmentClass);
	}

	return nullptr;
}

UGameplayEquipmentFragment* UGameplayInventoryItemInstance::FindEquipmentFragmentByClass(TSubclassOf<UGameplayEquipmentFragment> FragmentClass) const
{
	return nullptr; //@TODO: Implement
}

void UGameplayInventoryItemInstance::PostNetInit()
{
	// When being created due to replication, we need to find the owning component
	if (OwnerComponent != nullptr)
	{
		return;
	}

	AActor* OwnerActor = GetOwningActor();
	if (ensure(OwnerActor))
	{
		UGameplayInventoryManager* Manager = GetOwnerComponent<UGameplayInventoryManager>();
		if (ensure(Manager))
		{
			OwnerComponent = Manager;
		}
	}
}

AActor* UGameplayInventoryItemInstance::GetOwningActor()
{
	if (UObject* Outer = GetOuter())
	{
		return Cast<AActor>(Outer);
	}

	if (const UActorComponent* Component = GetOwnerComponent())
	{
		return Component->GetOwner();
	}

	return nullptr;
}
