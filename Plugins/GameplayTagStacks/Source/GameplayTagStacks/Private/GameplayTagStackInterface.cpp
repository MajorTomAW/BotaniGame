// Copyright © 2024 MajorT. All rights reserved.


#include "GameplayTagStackInterface.h"
#include "GameFramework/Actor.h"
#include "GameplayTagStackContainer.h"


UGameplayTagStackInterface::UGameplayTagStackInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const FGameplayTagStackContainer& IGameplayTagStackInterface::GetOwnedGameplayTagStacks() const
{
	static FGameplayTagStackContainer EmptyContainer = FGameplayTagStackContainer();
	return EmptyContainer;
}

FGameplayTagStackContainer IGameplayTagStackInterface::GetOwnedGameplayTagStacks_Copy() const
{
	return GetOwnedGameplayTagStacks();
}

bool IGameplayTagStackInterface::HasMatchingGameplayTagStack(const FGameplayTag TagToCheck) const
{
	return GetOwnedGameplayTagStacks().ContainsTag(TagToCheck);
}

int32 IGameplayTagStackInterface::GetGameplayTagStackCount(const FGameplayTag TagToCheck) const
{
	return GetOwnedGameplayTagStacks().GetStackCount(TagToCheck);
}

void IGameplayTagStackInterface::AddGameplayTagStack(const FGameplayTag Tag, const int32 StackCount)
{
	FGameplayTagStackContainer& MutableContainer = const_cast<FGameplayTagStackContainer&>(GetOwnedGameplayTagStacks());
	MutableContainer.AddStack(Tag, StackCount);

	// UE_LOG(LogTemp, Error, TEXT("Adding stack %s with count %d"), *Tag.ToString(), StackCount);
}

void IGameplayTagStackInterface::SetGameplayTagStack(FGameplayTag Tag, int32 StackCount)
{
	FGameplayTagStackContainer& MutableContainer = const_cast<FGameplayTagStackContainer&>(GetOwnedGameplayTagStacks());
	MutableContainer.SetStack(Tag, StackCount);
}

void IGameplayTagStackInterface::RemoveGameplayTagStack(const FGameplayTag Tag, const int32 StackCount)
{
	FGameplayTagStackContainer& MutableContainer = const_cast<FGameplayTagStackContainer&>(GetOwnedGameplayTagStacks());
	MutableContainer.RemoveStack(Tag, StackCount);
}

void UGameplayTagStackLibrary::GetOwnedGameplayTagStacks(UObject* Target, FGameplayTagStackContainer& TagStackContainer)
{
	// Check for the interface on the actor (Works for c++ implementations)
	if (const IGameplayTagStackInterface* TagStackInterface = Cast<IGameplayTagStackInterface>(Target))
	{
		TagStackContainer = TagStackInterface->GetOwnedGameplayTagStacks();
		return;
	}
	
	// Check for the interface on the actor (Works for blueprint implementations)
	if (Target->GetClass()->ImplementsInterface(UGameplayTagStackInterface::StaticClass()))
	{
		//@TODO: Implement blueprint version of GetOwnedGameplayTagStacks
		return;
	}
}

bool UGameplayTagStackLibrary::HasMatchingGameplayTagStack(UObject* Target, const FGameplayTag TagToCheck)
{
	// Check for the interface on the actor (Works for c++ implementations)
	if (const IGameplayTagStackInterface* TagStackInterface = Cast<IGameplayTagStackInterface>(Target))
	{
		return TagStackInterface->HasMatchingGameplayTagStack(TagToCheck);
	}

	// Check for the interface on the actor (Works for blueprint implementations)
	if (Target->GetClass()->ImplementsInterface(UGameplayTagStackInterface::StaticClass()))
	{
		//@TODO: Implement blueprint version of HasMatchingGameplayTagStack
	};

	return false;
}

bool UGameplayTagStackLibrary::HasMatchingGameplayTagStackContainer(const FGameplayTagStackContainer& TagStackContainer, const FGameplayTag TagToCheck)
{
	return TagStackContainer.ContainsTag(TagToCheck);
}

int32 UGameplayTagStackLibrary::GetGameplayTagStackCount(UObject* Target, const FGameplayTag TagToCheck)
{
	// Check for the interface on the actor (Works for c++ implementations)
	if (const IGameplayTagStackInterface* TagStackInterface = Cast<IGameplayTagStackInterface>(Target))
	{
		return TagStackInterface->GetGameplayTagStackCount(TagToCheck);
	}

	// Check for the interface on the actor (Works for blueprint implementations)
	if (Target && Target->GetClass()->ImplementsInterface(UGameplayTagStackInterface::StaticClass()))
	{
		//@TODO: Implement blueprint version of GetGameplayTagStackCount
	}

	return 0;
}

int32 UGameplayTagStackLibrary::GetGameplayTagStackCountContainer(const FGameplayTagStackContainer& TagStackContainer, const FGameplayTag TagToCheck)
{
	return TagStackContainer.GetStackCount(TagToCheck);
}

TArray<FGameplayTagStack> UGameplayTagStackLibrary::GetGameplayTagStacks(
	const FGameplayTagStackContainer& TagStackContainer)
{
	return TagStackContainer.GetStacks();
}

void UGameplayTagStackLibrary::BreakGameplayTagStack(
	const FGameplayTagStack& TagStack, FGameplayTag& Tag, int32& StackCount)
{
	Tag = TagStack.GetTag();
	StackCount = TagStack.GetStackCount();
}

