// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Pickup/PickupProxyActor.h"

#include "GameplayInventoryLogChannels.h"
#include "Components/SphereComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Net/UnrealNetwork.h"

#include "Definitions/GameplayInventoryItemDefinition.h"
#include "Icons/PickupItemIcon.h"
#include "Net/Core/PushModel/PushModel.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(PickupProxyActor)

APickupProxyActor::APickupProxyActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bItemPickupEffectsPlayed(false)
{
	/** Initialize the pickup proxy actor */
	bReplicates = true;
	bNetLoadOnClient = true;
	NetUpdateFrequency = 40.f;
	//NetCullDistanceSquared = 112000;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);

	CollisionComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollisionComponent"));
	ensure(CollisionComponent);
	CollisionComponent->InitSphereRadius(40.f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnPickupCollisionBeginOverlap);
	SetRootComponent(CollisionComponent);

	RotationSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RotationSceneComponent"));
	ensure(RotationSceneComponent);
	RotationSceneComponent->SetupAttachment(CollisionComponent);
}

void APickupProxyActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_InitialOnly;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ItemDefinition, Params);
}

void APickupProxyActor::BeginPlay()
{
	Super::BeginPlay();

	if (ItemDefinition.IsNull())
	{
		return; //@TODO: should the client know about the item def? (80% no)
	}

	const UItemFragment_PickupDefinition* PickupFrag = ItemDefinition.LoadSynchronous()->GetItemFragment<UItemFragment_PickupDefinition>();
	ApplyPickupFragment(PickupFrag);
	if (!ensure(PickupFrag))
	{
		return;
	}
	
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ItemDefinition, this);

	//@TODO: Cosmetics

	if (!GWorld->IsEditorWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(UnloadItemTimerHandle, this, &ThisClass::RequestUnloadItem, UnloadItemDelay, false);
	}

	OnDropped(GetInstigator(), ItemDefinition.Get(), PickupFrag->PickupData);
}

void APickupProxyActor::OnDropped(AActor* InInstigator, UGameplayInventoryItemDefinition* InItemDefinition, const FInventoryItemPickupData& InPickupData)
{
	for (UPickupItemIcon* Icon : InPickupData.PickupIcons)
	{
		Icon->ApplyPickupIcon(this, InItemDefinition);
	}

	K2_OnDropped(InInstigator, InItemDefinition, InPickupData);
}

void APickupProxyActor::OnPickedUp(AActor* InInstigator)
{
	K2_OnPickedUp(InInstigator);
}

UGameplayInventoryItemDefinition* APickupProxyActor::GetItemDefinition() const
{
	UGameplayInventoryItemDefinition* ItemDef = ItemDefinition.Get();
	if (ItemDef == nullptr)
	{
		ItemDef = ItemDefinition.LoadSynchronous();
	}
	return ItemDef;
}

void APickupProxyActor::OnPickupCollisionBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void APickupProxyActor::ApplyPickupFragment(const UItemFragment_PickupDefinition* InPickupFragment)
{
	if (InPickupFragment)
	{
		CollisionComponent->SetSphereRadius(InPickupFragment->PickupData.PickupRadius);
	}
	else
	{
		CollisionComponent->SetSphereRadius(40.f);
	}

	// Delete any attached icons
	TArray<USceneComponent*> AttachedIcons;
	RotationSceneComponent->GetChildrenComponents(true, AttachedIcons);
	for (auto It = AttachedIcons.CreateIterator(); It; ++It)
	{
		USceneComponent* Icon = *It;

		if (Icon == nullptr)
		{
			continue;
		}

		if (Icon->IsDefaultSubobject())
		{
			continue;
		}
		
		Icon->DestroyComponent();
	}
}

#if WITH_EDITOR
void APickupProxyActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (ItemDefinition.IsNull())
	{
		ApplyPickupFragment(nullptr);
		return;
	}

	UItemFragment_PickupDefinition* PickupFrag = ItemDefinition.LoadSynchronous()->GetItemFragment<UItemFragment_PickupDefinition>();
	ApplyPickupFragment(PickupFrag);
	if (PickupFrag == nullptr)
	{
		return;
	}

	// Only apply the icons in the editor preview. (not in PIE or standalone)
	if (GetInstigator())
	{
		return;
	}
	
	for (UPickupItemIcon* Icon : PickupFrag->PickupData.PickupIcons)
	{
		Icon->ApplyPickupIcon(this, ItemDefinition.Get());
	}
}
#endif

void APickupProxyActor::OnRep_ItemDefinition()
{
	OnDropped(GetInstigator(), ItemDefinition.Get(), GetPickupData());
	
	if (GetPickupData().IsValid())
	{
		// Play item pickup effects
	}
	else
	{
		
	}
}

FInventoryItemPickupData APickupProxyActor::GetPickupData() const
{
	UItemFragment_PickupDefinition* PickupFrag = ItemDefinition.LoadSynchronous()->GetItemFragment<UItemFragment_PickupDefinition>();
	if (!ensure(PickupFrag))
	{
		return FInventoryItemPickupData();
	}

	return PickupFrag->PickupData;
}

void APickupProxyActor::RequestUnloadItem()
{
	/*if (ItemDefinitionPath.IsNull())
	{
		ItemDefinitionPath = ItemDefinition.Get();
	}

	ItemDefinition.Reset();

	LOG_INVENTORY(Log, TEXT("Unloading item definition %s."), *ItemDefinitionPath.ToString());
	
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.Unload(ItemDefinitionPath);
	Streamable.Unload(ItemDefinition.Get());*/
}
