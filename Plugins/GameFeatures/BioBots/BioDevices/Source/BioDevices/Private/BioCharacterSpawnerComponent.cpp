// Copyright © 2024 Botanibots Team. All rights reserved.


#include "BioCharacterSpawnerComponent.h"

#include "Character/Components/BotaniHealthComponent.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BioCharacterSpawnerComponent)

UBioCharacterSpawnerComponent::UBioCharacterSpawnerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SpawnCount = 1;
	BotDefinition = nullptr;
	bDespawnAIOnDisable = false;
}

void UBioCharacterSpawnerComponent::StartSpawning()
{
	// Only the server should be able to start spawning
	if (GetOwner()->GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	
	// If we are already spawning, do nothing
	if (SpawnTimerHandle.IsValid())
	{
		return;
	}

	UWorld* World = GetWorld();
	check(World);

	World->GetTimerManager().SetTimer(SpawnTimerHandle,
		FTimerDelegate::CreateUObject(this, &ThisClass::TimerCallSpawnAI),
		1.f,
		true,
		1.f);
	
}

void UBioCharacterSpawnerComponent::StopSpawning()
{
	// Only the server should be able to stop spawning
	if (GetOwner()->GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	
	UWorld* World = GetWorld();
	check(World);

	if (SpawnTimerHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(SpawnTimerHandle);
		SpawnTimerHandle.Invalidate();
	}

	if (bDespawnAIOnDisable)
	{
		for (auto& Key : SpawnedAIs)
		{
			UObject* Obj = Key.ResolveObjectPtr();
			
			if (Obj == nullptr)
			{
				continue;
			}

			if (UBotaniHealthComponent* HealthComponent = UBotaniHealthComponent::FindHealthComponent(Cast<AActor>(Obj)))
			{
				HealthComponent->DamageSelfDestruct(true);
			}
			else
			{
				Obj->ConditionalBeginDestroy();
			}
		}

		SpawnedAIs.Empty();
	}
}

void UBioCharacterSpawnerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UBioCharacterSpawnerComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
}

FVector UBioCharacterSpawnerComponent::GetSpawnLocation() const
{
	if (OnGetSpawnLocation.IsBound())
	{
		return OnGetSpawnLocation.Execute();
	}

	return GetOwner()->GetActorLocation();
}

void UBioCharacterSpawnerComponent::TimerCallSpawnAI()
{
	const FVector SpawnLocation = GetSpawnLocation();
	const FRotator SpawnRotation = FRotator::ZeroRotator;

	SpawnAI(SpawnLocation, SpawnRotation);
}

void UBioCharacterSpawnerComponent::SpawnAI(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	
}
