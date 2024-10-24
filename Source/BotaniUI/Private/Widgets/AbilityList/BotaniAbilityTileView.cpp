// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Widgets/AbilityList/BotaniAbilityTileView.h"

#include "AbilitySystemComponent.h"
#include "BotaniLogChannels.h"
#include "AbilitySystem/BotaniAbilitySystemComponent.h"
#include "GameplayTags/BotaniGameplayTags.h"
#include "Widgets/AbilityList/BotaniAbilityTileViewEntry.h"

UBotaniAbilityTileEntryData::UBotaniAbilityTileEntryData()
	: AbilitySpec(nullptr)
{
}

void UBotaniAbilityTileEntryData::Initialize(const FGameplayAbilitySpec& Spec)
{
	AbilitySpec = &Spec;
	Handle = Spec.Handle;
	
	for (auto& Tag : Spec.DynamicAbilityTags.GetGameplayTagArray())
	{
		if (!Tag.IsValid())
		{
			continue;
		}

		if (!Tag.MatchesTag(BotaniGameplayTags::Input::TAG_InputTag))
		{
			continue;
		}

		InputTags.AddTag(Tag);
	}
}


UBotaniAbilityTileView::UBotaniAbilityTileView(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, AbilitySystemComponent(nullptr)
{
	TileAlignment = EListItemAlignment::EvenlyDistributed;
	Orientation = Orient_Horizontal;
	SetScrollbarVisibility(ESlateVisibility::Collapsed);
}

void UBotaniAbilityTileView::InitializeWithAbilitySystem(UAbilitySystemComponent* InAbilitySystemComponent)
{
	check(InAbilitySystemComponent);
	
	if (AbilitySystemComponent == InAbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent = InAbilitySystemComponent;
	RegisterAbilitySystemEvents();
	AddInitialEntries();
}

void UBotaniAbilityTileView::UninitializeAbilitySystem()
{
	if (AbilitySystemComponent == nullptr)
	{
		return;
	}

	UnregisterAbilitySystemEvents();

	AbilitySystemComponent = nullptr;
}

void UBotaniAbilityTileView::AddInitialEntries()
{
	check(AbilitySystemComponent);

	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		AddAbilitySpec(Spec);
	}
}

void UBotaniAbilityTileView::AddAbilitySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (FindEntryDataByHandle(AbilitySpec.Handle) != nullptr)
	{
		return;
	}

	if (const UBotaniGameplayAbility* BotaniAbility = Cast<UBotaniGameplayAbility>(AbilitySpec.Ability))
	{
		if (BotaniAbility->GetHUDPolicy() == EBotaniAbilityHUDPolicy::None)
		{
			return;
		}
	}

	UBotaniAbilityTileEntryData* EntryData = NewObject<UBotaniAbilityTileEntryData>(this);
	EntryData->Initialize(AbilitySpec);

	AddItem(EntryData);
}

void UBotaniAbilityTileView::RegisterAbilitySystemEvents()
{
	check(AbilitySystemComponent);

	AbilitySystemComponent->RegisterGenericGameplayTagEvent().AddUObject(this, &ThisClass::OnGameplayTagChanged);
	AbilitySystemComponent->AbilityActivatedCallbacks.AddUObject(this, &ThisClass::OnAbilityActivated);
	AbilitySystemComponent->AbilityEndedCallbacks.AddUObject(this, &ThisClass::OnAbilityEnded);

	if (UBotaniAbilitySystemComponent* BotaniASC = Cast<UBotaniAbilitySystemComponent>(AbilitySystemComponent))
	{
		BotaniASC->OnAbilitiesReplicatedCallbacks.AddUObject(this, &ThisClass::OnAbilitiesReplicated);
	}
}

void UBotaniAbilityTileView::UnregisterAbilitySystemEvents()
{
	if (AbilitySystemComponent == nullptr)
	{
		return;
	}

	AbilitySystemComponent->RegisterGenericGameplayTagEvent().RemoveAll(this);
	AbilitySystemComponent->AbilityActivatedCallbacks.RemoveAll(this);
	AbilitySystemComponent->AbilityEndedCallbacks.RemoveAll(this);

	if (UBotaniAbilitySystemComponent* BotaniASC = Cast<UBotaniAbilitySystemComponent>(AbilitySystemComponent))
	{
		BotaniASC->OnAbilitiesReplicatedCallbacks.RemoveAll(this);
	}
}

void UBotaniAbilityTileView::OnGameplayTagChanged(const FGameplayTag GameplayTag, int32 Count)
{
	for (UObject* ListItem : GetListItems())
	{
		if (ListItem == nullptr)
		{
			continue;
		}

		const UBotaniAbilityTileEntryData* EntryData = Cast<UBotaniAbilityTileEntryData>(ListItem);
		UBotaniAbilityTileViewEntry* Entry = GetEntryWidgetFromItem<UBotaniAbilityTileViewEntry>(EntryData);
		if (Entry == nullptr)
		{
			return;
		}

		Entry->OnGameplayTagChanged(GameplayTag, Count);
	}
}

void UBotaniAbilityTileView::OnAbilityActivated(UGameplayAbility* Ability)
{
	check(Ability);

	const UBotaniAbilityTileEntryData* EntryData = FindEntryDataByHandle(Ability->GetCurrentAbilitySpecHandle());
	if (EntryData == nullptr)
	{
		return;
	}

	UBotaniAbilityTileViewEntry* Entry = GetEntryWidgetFromItem<UBotaniAbilityTileViewEntry>(EntryData);
	if (Entry == nullptr)
	{
		return;
	}

	Entry->OnAbilityActivated(Ability);
}

void UBotaniAbilityTileView::OnAbilityEnded(UGameplayAbility* Ability)
{
	check(Ability);

	const UBotaniAbilityTileEntryData* EntryData = FindEntryDataByHandle(Ability->GetCurrentAbilitySpecHandle());
	if (EntryData == nullptr)
	{
		return;
	}

	UBotaniAbilityTileViewEntry* Entry = GetEntryWidgetFromItem<UBotaniAbilityTileViewEntry>(EntryData);
	if (Entry == nullptr)
	{
		return;
	}

	Entry->OnAbilityEnded(Ability);
}

void UBotaniAbilityTileView::OnAbilitiesReplicated(const TArray<FGameplayAbilitySpec>& Abilities)
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	
	// Remove existing entries for any abilities that got removed.
	for (auto It = GetListItems().CreateConstIterator(); It; ++It)
	{
		auto* EntryData = Cast<UBotaniAbilityTileEntryData>(*It);
		if (EntryData == nullptr)
		{
			continue;
		}

		bool bFound = false;
		for (const auto& AbilitySpec : Abilities)
		{
			if ((AbilitySpec.Handle == EntryData->Handle) &&
				(EntryData->AbilitySpec != nullptr) &&
				(ASC->FindAbilitySpecFromHandle(EntryData->Handle) != nullptr))
			{
				bFound = true;
				break;
			}
		}

		if (!bFound)
		{
			RemoveItem(EntryData);
		}
		else
		{
			BOTANI_UI_LOG(Display, TEXT("Ability %s was not removed"), *EntryData->Handle.ToString());
		}
	}
	
	
	// Add new entries for any abilities that were replicated.
	for (const FGameplayAbilitySpec& AbilitySpec : Abilities)
	{
		AddAbilitySpec(AbilitySpec);
	}
}

UBotaniAbilityTileEntryData* UBotaniAbilityTileView::FindEntryDataByHandle(const FGameplayAbilitySpecHandle Handle) const
{
	for (auto& Entry : GetListItems())
	{
		auto* EntryData = Cast<UBotaniAbilityTileEntryData>(Entry);
		if (EntryData && EntryData->Handle == Handle)
		{
			return EntryData;
		}
	}

	return nullptr;
}
