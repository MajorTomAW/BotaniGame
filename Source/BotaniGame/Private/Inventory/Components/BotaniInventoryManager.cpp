// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/Components/BotaniInventoryManager.h"

#include "Engine/Canvas.h"
#include "GameFramework/PlayerState.h"
#include "Instance/GameplayInventoryItemInstance.h"
#include "Inventory/Components/BotaniQuickBarComponent.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniInventoryManager)

namespace BotaniConsoleVariables
{
	static bool DrawInventoryDebug = false;
	static FAutoConsoleVariableRef CVarDrawInventoryDebug(
		TEXT("botani.Inventory.DrawDebug"),
		DrawInventoryDebug,
		TEXT("Draws debug information for the inventory system."),
		ECVF_Default
	);

	bool ShouldDrawInventoryDebug()
	{
		return DrawInventoryDebug;
	}
}

UBotaniInventoryManager::UBotaniInventoryManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AHUD::OnShowDebugInfo.AddStatic(&UBotaniInventoryManager::OnShowInventoryDebugInfo);
}

UBotaniInventoryManager* UBotaniInventoryManager::GetInventoryManager(AActor* Actor)
{
	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		if (UBotaniInventoryManager* InventoryManager = Pawn->FindComponentByClass<UBotaniInventoryManager>())
		{
			return InventoryManager;
		}
		
		if (UBotaniInventoryManager* InventoryManager = Pawn->GetController()->FindComponentByClass<UBotaniInventoryManager>())
		{
			return InventoryManager;
		}
	}

	if (AController* Controller = Cast<AController>(Actor))
	{
		if (UBotaniInventoryManager* InventoryManager = Controller->FindComponentByClass<UBotaniInventoryManager>())
		{
			return InventoryManager;
		}
	}

	if (APlayerState* PlayerState = Cast<APlayerState>(Actor))
	{
		if (UBotaniInventoryManager* InventoryManager = PlayerState->GetOwningController()->FindComponentByClass<UBotaniInventoryManager>())
		{
			return InventoryManager;
		}
	}

	return nullptr;
}

void UBotaniInventoryManager::OnRemoveItem(FGameplayInventoryItemSpec& InSpec)
{
	if (!InSpec.GetInstance())
	{
		return;
	}

	// Notify the quick bar component that the item has been removed
	if (UBotaniQuickBarComponent* QuickBar = GetOwner()->FindComponentByClass<UBotaniQuickBarComponent>())
	{
		QuickBar->RemoveItemByHandle(InSpec.GetHandle());
	}

	OnItemRemoved.Broadcast(InSpec.GetHandle());

	// Notify the item instance that it has been removed
	FGameplayInventoryItemContext Context(GetOwner());
	Context.InventoryComponent = this;
	Context.ItemDefinition = InSpec.GetItemDefinition();
	Context.ItemInstance = InSpec.GetInstance();
	
	UGameplayInventoryItemInstance* Instance = InSpec.GetInstance();
	if (ensure(Instance))
	{
		Instance->OnInstanceRemoved(InSpec.GetHandle(), &Context);

		if (GetOwnerRole() == ROLE_Authority)
		{
			Instance->MarkAsGarbage();
		}
	}

	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn && OwnerPawn->IsLocallyControlled() && OwnerPawn->HasAuthority())
	{
		BroadcastInventoryChangeMessage(Instance, InSpec.GetStackCount(), 0);
	}
}

void UBotaniInventoryManager::OnShowInventoryDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos)
{
	if (!BotaniConsoleVariables::ShouldDrawInventoryDebug())
	{
		return;
	}
	
	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetFont(GEngine->GetSmallFont());
	DisplayDebugManager.SetDrawColor(FColor::White);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("InventoryManager: %s"), *GetNameSafe(UBotaniInventoryManager::StaticClass())));
}
