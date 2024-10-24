// Copyright © 2024 MajorT. All rights reserved.


#include "Instance/GameplayEquipmentInstance.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif

#include "GameplayInventoryLogChannels.h"
#include "Definitions/GameplayEquipmentDefinition.h"
#include "Definitions/GameplayInventoryItemDefinition.h"
#include "Equipment/GameplayEquipmentItem.h"
#include "Fragments/GameplayEquipmentFragment.h"
#include "GameFramework/Character.h"
#include "Instance/GameplayInventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayEquipmentInstance)

UGameplayEquipmentInstance::UGameplayEquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGameplayEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	if (const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
	{
		BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
	}

	DOREPLIFETIME(ThisClass, Instigator);
	DOREPLIFETIME(ThisClass, CurrentContext);
}

UWorld* UGameplayEquipmentInstance::GetWorld() const
{
	if (const APawn* OwningPawn = GetPawn())
	{
		return OwningPawn->GetWorld();
	}

	return nullptr;
}

int32 UGameplayEquipmentInstance::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	check(GetOuter() != nullptr);
	return GetOuter()->GetFunctionCallspace(Function, Stack);
}

bool UGameplayEquipmentInstance::CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack)
{
	check(!HasAnyFlags(RF_ClassDefaultObject));

	AActor* Owner = GetTypedOuter<AActor>();
	UNetDriver* NetDriver = Owner->GetNetDriver();
	if (NetDriver)
	{
		NetDriver->ProcessRemoteFunction(Owner, Function, Parms, OutParms, Stack, this);
		return true;
	}

	return false;
}

APawn* UGameplayEquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

APawn* UGameplayEquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnClass) const
{
	APawn* Result = nullptr;
	if (UClass* ActualPawnClass = PawnClass)
	{
		if (GetOuter()->IsA(ActualPawnClass))
		{
			Result = Cast<APawn>(GetOuter());
		}
	}

	return Result;
}

UGameplayInventoryItemDefinition* UGameplayEquipmentInstance::GetItemDefinition() const
{
	const UGameplayInventoryItemInstance* ItemInstance = Cast<UGameplayInventoryItemInstance>(Instigator);
	return ItemInstance ? ItemInstance->GetItemDefinition() : nullptr;
}

UGameplayEquipmentDefinition* UGameplayEquipmentInstance::GetEquipmentDefinition() const
{
	UGameplayInventoryItemDefinition* ItemDef = GetItemDefinition();
	return ItemDef ? ItemDef->EquipmentDefinition : nullptr;
}

void UGameplayEquipmentInstance::OnEquipped(const FGameplayEquipmentSpec& EquipmentSpec, const FGameplayInventoryItemContext& Context)
{
	// Cache the context
	CurrentContext = Context;
	
	SpawnEquipmentActors(EquipmentSpec.EquipmentDefinition->ActorSpawnRule->GetActorSpawnQueries(GetPawn()));

	FGameplayTagContainer BlockingTags;

	for (const UGameplayEquipmentFragment* Fragment : EquipmentSpec.EquipmentDefinition->EquipmentFragments)
	{
		LOG_INVENTORY(Display, TEXT("EQUIP: Executing Fragment %s"), *Fragment->GetName());
		
		if (Fragment == nullptr)
		{
			continue;
		}

		if (!Fragment->CanApplyInventoryFragment(Context, EquipmentSpec.Handle, &BlockingTags))
		{
			LOG_INVENTORY(Display, TEXT("EQUIP: Fragment %s cannot be applied to equipment %s"), *Fragment->GetName(), *EquipmentSpec.Handle.ToString());
			continue;
		}

		Fragment->OnInstanceCreated(Context, EquipmentSpec.Handle, this);
	}

	K2_OnEquipped(EquipmentSpec, EquipmentSpec.Handle, Context);
}

void UGameplayEquipmentInstance::OnUnequipped(const FGameplayEquipmentSpec& EquipmentSpec)
{
	FGameplayTagContainer BlockingTags;
	for (const UGameplayEquipmentFragment* Fragment : EquipmentSpec.EquipmentDefinition->EquipmentFragments)
	{
		LOG_INVENTORY(Display, TEXT("UNEQUIP: Executing Fragment %s"), *Fragment->GetName());
		
		if (Fragment == nullptr)
		{
			continue;
		}

		if (!Fragment->CanApplyInventoryFragment(CurrentContext, EquipmentSpec.Handle, &BlockingTags))
		{
			LOG_INVENTORY(Display, TEXT("UNEQUIP: Fragment %s cannot be applied to equipment %s"), *Fragment->GetName(), *EquipmentSpec.Handle.ToString());
			continue;
		}

		Fragment->OnInstanceDestroyed(CurrentContext, EquipmentSpec.Handle, this);
	}

	DestroyEquipmentActors();
	
	K2_OnUnequipped(EquipmentSpec, EquipmentSpec.Handle);
}

void UGameplayEquipmentInstance::SpawnEquipmentActors(const TArray<FGameplayEquipmentActorSpawnQuery>& Queries)
{

	for (const auto& Query : Queries)
	{
		if (!Query.IsValid())
		{
			continue;
		}

		if (Query.ActorToSpawn.ActorToSpawn.IsNull())
		{
			continue;
		}

		UClass* ActorClass = Query.ActorToSpawn.ActorToSpawn.LoadSynchronous();
		AGameplayEquipmentItem* NewActor = GetWorld()->SpawnActorDeferred<AGameplayEquipmentItem>(ActorClass, FTransform::Identity, GetPawn());
		NewActor->FinishSpawning(FTransform::Identity, true);
		check(NewActor);

		NewActor->SetActorRelativeTransform(Query.ActorToSpawn.AttachTransform);
		NewActor->AttachToComponent(Query.AttachTarget.Get(), FAttachmentTransformRules::KeepRelativeTransform, Query.ActorToSpawn.AttachSocket);
		NewActor->OnEquipped(GetPawn(), CurrentContext);

		LOG_INVENTORY(Log, TEXT("Attached Query: %s to component: %s with socket: %s"), *NewActor->GetName(), *Query.AttachTarget->GetName(), *Query.ActorToSpawn.AttachSocket.ToString());

		SpawnedEquipmentActors.Add(NewActor);
	}
}

void UGameplayEquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedEquipmentActors)
	{
		if (Actor == nullptr)
		{
			continue;
		}

		AGameplayEquipmentItem* EquipmentActor = Cast<AGameplayEquipmentItem>(Actor);
		if (EquipmentActor)
		{
			EquipmentActor->OnUnequipped(GetPawn(), CurrentContext);
		}

		Actor->Destroy();
	}
}

#if UE_WITH_IRIS
void UGameplayEquipmentInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;

	// Build descriptors and allocate PropertyReplicationFragments for this object
	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}

void UGameplayEquipmentInstance::OnRep_Instigator()
{
}
#endif
