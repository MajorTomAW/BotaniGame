// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/Pickup/BotaniPickupProxy.h"

#include "BotaniLogChannels.h"
#include "NiagaraComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Inventory/BotaniInventoryStatics.h"
#include "Inventory/Components/BotaniInventoryManager.h"
#include "Inventory/Components/BotaniQuickBarComponent.h"
#include "Inventory/Data/BotaniRarityStyleAsset.h"
#include "Inventory/Definitions/BotaniInventoryItemDefinition.h"
#include "Inventory/Instances/BotaniItemInstance.h"
#include "System/BotaniAssetManager.h"
#include "Weapons/Components/BotaniProjectileMovementComponent.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniPickupProxy)

ABotaniPickupProxy::ABotaniPickupProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InitialMovement = EBotaniInitialItemMovement::Gravity;
	InitialInteractionDelay = .5f;
	
	PickupMovement = ObjectInitializer.CreateDefaultSubobject<UBotaniProjectileMovementComponent>(this, TEXT("PickupMovement"));
	ensure(PickupMovement);
	PickupMovement->SetUpdatedComponent(CollisionComponent);
	PickupMovement->bShouldBounce = true;
	PickupMovement->Bounciness = 0.5f;
	PickupMovement->Friction = 0.5f;
	PickupMovement->MaxSpeed = 2000.f;
	PickupMovement->InitialSpeed = 400.f;
	PickupMovement->Velocity = FVector(1.f, 0.f, 0.25f);

	PickupLight = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, TEXT("PickupLight"));
	ensure(PickupLight);
	PickupLight->SetupAttachment(RotationSceneComponent);

	PickupGlowSystem = ObjectInitializer.CreateDefaultSubobject<UNiagaraComponent>(this, TEXT("PickupGlowSystem"));
	ensure(PickupGlowSystem);
	PickupGlowSystem->SetupAttachment(RotationSceneComponent);
}

void ABotaniPickupProxy::BeginPlay()
{
	if (InitialMovement == EBotaniInitialItemMovement::None)
	{
		DisablePickupMovement();
	}
	else if (InitialMovement == EBotaniInitialItemMovement::Gravity)
	{
		EnablePickupMovement_Gravity();
	}

	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	
	Super::BeginPlay();

	auto EnablePickupCollision = [this]()
	{
		CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

		// Update the overlap events
		CollisionComponent->UpdateOverlaps();
	};

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda(EnablePickupCollision), InitialInteractionDelay, false);
}

void ABotaniPickupProxy::OnDropped(AActor* InInstigator, UGameplayInventoryItemDefinition* InItemDefinition, const FInventoryItemPickupData& InPickupData)
{
	const FString NetworkRoleString = UEnum::GetValueAsString(InInstigator ? InInstigator->GetLocalRole() : GetLocalRole());
	BOTANI_LOG(Display, TEXT("%s, Item: %s"), *NetworkRoleString, *GetNameSafe(InItemDefinition));

	if (UBotaniInventoryItemDefinition* BotaniItemDef = Cast<UBotaniInventoryItemDefinition>(InItemDefinition))
	{
		EBotaniItemRarity ItemRarity = BotaniItemDef->Rarity;
		const FString RarityString = UEnum::GetValueAsString(ItemRarity);
		BOTANI_LOG(Display, TEXT("%s, Rarity: %s"), *NetworkRoleString, *RarityString);

		UBotaniAssetManager& AssetManager = UBotaniAssetManager::Get();
		check(!AssetManager.GetGameData().BotaniRarityData.IsNull());

		UBotaniRarityStyleAsset* StyleAsset = AssetManager.GetGameData().BotaniRarityData.Get();
		if (StyleAsset == nullptr)
		{
			StyleAsset = AssetManager.GetAsset<UBotaniRarityStyleAsset>(AssetManager.GetGameData().BotaniRarityData, true);
		}

		FBotaniRarityStyleInfo Style = StyleAsset->GetRarityStyle(ItemRarity);

		// Update the light
		if (const FLinearColor* Color = Style.ColorParameters.Find("Color"))
		{
			PickupLight->SetLightColor(*Color);
		}
		else
		{
			PickupLight->SetLightColor(FLinearColor::White);
		}

		// Update the glow system
		check(PickupGlowSystem);
		for (const auto& KVP : Style.ColorParameters)
		{
			PickupGlowSystem->SetVariableLinearColor(KVP.Key, KVP.Value);
		}
		for (const auto& KVP : Style.ScalarParameters)
		{
			PickupGlowSystem->SetVariableFloat(KVP.Key, KVP.Value);
		}
	}

	// Mesh and so
	UItemFragment_PickupDefinition* PickupFragment = InItemDefinition->GetItemFragment<UItemFragment_PickupDefinition>();
	if (ensure(PickupFragment))
	{
	}
	
	Super::OnDropped(InInstigator, InItemDefinition, InPickupData);
}

void ABotaniPickupProxy::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (ItemDefinition.IsNull())
	{
		return;
	}

	if (UBotaniInventoryItemDefinition* BotaniItemDef = Cast<UBotaniInventoryItemDefinition>(ItemDefinition.LoadSynchronous()))
	{
		EBotaniItemRarity ItemRarity = BotaniItemDef->Rarity;
		UBotaniAssetManager& AssetManager = UBotaniAssetManager::Get();

		if (IsValid(&AssetManager.GetGameData()))
		{
			UBotaniRarityStyleAsset* StyleAsset = AssetManager.GetGameData().BotaniRarityData.Get();
			if (StyleAsset == nullptr)
			{
				StyleAsset = AssetManager.GetAsset<UBotaniRarityStyleAsset>(AssetManager.GetGameData().BotaniRarityData, true);
			}

			FBotaniRarityStyleInfo Style = StyleAsset->GetRarityStyle(ItemRarity);
			
			// Update the light
			if (const FLinearColor* Color = Style.ColorParameters.Find("Color"))
			{
				PickupLight->SetLightColor(*Color);
			}
			else
			{
				PickupLight->SetLightColor(FLinearColor::White);
			}

			// Update the glow system
			check(PickupGlowSystem);
			for (const auto& KVP : Style.ColorParameters)
			{
				PickupGlowSystem->SetVariableLinearColor(KVP.Key, KVP.Value);
			}
			for (const auto& KVP : Style.ScalarParameters)
			{
				PickupGlowSystem->SetVariableFloat(KVP.Key, KVP.Value);
			}
		}
	}
}

void ABotaniPickupProxy::OnPickupCollisionBeginOverlap_Implementation(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || OtherActor == nullptr)
	{
		return;
	}

	UGameplayInventoryItemDefinition* ItemDef = GetItemDefinition();
	UItemFragment_PickupDefinition* PickupFrag = ItemDef ? ItemDef->GetItemFragment<UItemFragment_PickupDefinition>() : nullptr;
	if (PickupFrag == nullptr)
	{
		return;
	}

	// If the item is not set to auto pickup, then return
	if (PickupFrag->PickupData.AutoPickupBehavior == EInventoryItemAutoPickupBehavior::DoNotAutoPickup)
	{
		return;
	}

	APawn* Pawn = Cast<APawn>(OtherActor);
	if (Pawn == nullptr)
	{
		return;
	}

	// Find the inventory manager
	UBotaniInventoryManager* InventoryManager = UBotaniInventoryManager::GetInventoryManager(Pawn);
	if (InventoryManager == nullptr)
	{
		return;
	}

	const FGameplayInventoryItemContext Context = InventoryManager->MakeItemContext(ItemDef, PickupQuantity, GetInstigator());
	const FGameplayInventoryItemSpec NewSpec = FGameplayInventoryItemSpec(Context.ItemDefinition, Context.StackCount, Context.Instigator);
	
	if (!InventoryManager->CanAddItemDef(NewSpec, Context))
	{
		return;
	}
	
	FGameplayInventoryItemSpecHandle Handle = InventoryManager->GiveItem(NewSpec, Context);

	if (!Handle.IsValid())
	{
		return;
	}

	// Equip the item if it is set to auto equip
	if (PickupFrag->PickupData.bAutoEquip)
	{
		if (UBotaniQuickBarComponent* QuickBar = UBotaniInventoryStatics::FindQuickBarComponent(Pawn->GetController()))
		{
			const int32 SlotIdx = QuickBar->GetNextFreeItemSlot();

			if (SlotIdx != INDEX_NONE)
			{
				QuickBar->AddItemToSlot(SlotIdx, Handle);
				QuickBar->SetActiveSlotIndex(SlotIdx);
			}
		}
	}

	// Apply stored tag stacks
	IGameplayTagStackInterface* TagStackPtr = Cast<IGameplayTagStackInterface>(InventoryManager->FindItemSpecFromHandle(Handle)->GetInstance());
	for (const auto& Stack : TagStacks.GetStacks())
	{
		TagStackPtr->SetGameplayTagStack(Stack.GetTag(), Stack.GetStackCount());
	}
	
	OnPickedUp(Pawn);
}

void ABotaniPickupProxy::DisablePickupMovement()
{
	check(PickupMovement);

	PickupMovement->Deactivate();
	PickupMovement->StopMovementImmediately();
}

void ABotaniPickupProxy::EnablePickupMovement_Gravity()
{
	check(PickupMovement);

	PickupMovement->InitialSpeed = 0.f;
	PickupMovement->Velocity = FVector(0.f, 0.f, -1.f);
}