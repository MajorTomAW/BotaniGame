// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniLogChannels.h"
#include "Character/BotaniCharacter.h"
#include "Definitions/GameplayEquipmentDefinition.h"
#include "BotaniEquipmentDefinition.generated.h"

/**
 * FBotaniEquipmentActorToSpawn
 */
USTRUCT(BlueprintType)
struct FBotaniEquipmentActorToSpawn
{
	GENERATED_BODY()

	FBotaniEquipmentActorToSpawn()
		: bUseAutonomousProxyComponent(true)
	{
		AttachComponent = "ThirdPersonMesh";
		AutonomousProxyAttachComponent = "FirstPersonMesh";
	}

public:
	/** The actor to spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSoftClassPtr<AGameplayEquipmentItem> ActorToSpawn;

	/** The component name to attach the actor to */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	FName AttachComponent;

	/** The socket to attach the actor to */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment", meta = (AnimNotifyBoneName = "true"))
	FName AttachSocket;

	/** The transform to attach the actor with */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	FTransform AttachTransform;

	/** Whether to use a different component to attach the actor to on the autonomous proxy */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	uint8 bUseAutonomousProxyComponent : 1;

	/** The component name to attach the actor to on the autonomous proxy */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment", meta = (EditCondition = "bUseAutonomousProxyComponent", EditConditionHides))
	FName AutonomousProxyAttachComponent;

	/** The socket to attach the actor to on the autonomous proxy */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment", meta = (EditCondition = "bUseAutonomousProxyComponent", AnimNotifyBoneName = "true", EditConditionHides))
	FName AutonomousProxyAttachSocket;

	/** The transform to attach the actor with on the autonomous proxy */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment", meta = (EditCondition = "bUseAutonomousProxyComponent", EditConditionHides))
	FTransform AutonomousProxyAttachTransform;
};

/**
 * UEquipmentActorToSpawnRule_Botani
 */
UCLASS(meta = (DisplayName = "Spawn Rule - Botani"))
class UEquipmentActorToSpawnRule_Botani : public UGameplayEquipmentActorToSpawnRule
{
	GENERATED_BODY()
	
public:
	/** Actors to spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TArray<FBotaniEquipmentActorToSpawn> ActorsToSpawn;

public:
	virtual TArray<FGameplayEquipmentActorSpawnQuery> GetActorSpawnQueries(const APawn* Pawn) const override
	{
		check(Pawn);

		USceneComponent* AttachTarget = Pawn->GetRootComponent();

		if (const ACharacter* Character = Cast<ACharacter>(Pawn))
		{
			AttachTarget = Character->GetMesh();
		}

		const ABotaniCharacter* BotaniCharacter = Cast<ABotaniCharacter>(Pawn);

		TArray<FGameplayEquipmentActorSpawnQuery> Queries;

		for (const auto& Spawn : ActorsToSpawn)
		{
			FGameplayEquipmentActorSpawnQuery Query;

			// Construct a default actor to spawn
			FGameplayEquipmentActorToSpawn ActorToSpawn;
			ActorToSpawn.ActorToSpawn = Spawn.ActorToSpawn;
			ActorToSpawn.AttachSocket = Spawn.AttachSocket;
			ActorToSpawn.AttachTransform = Spawn.AttachTransform;

			if (!Spawn.AttachComponent.IsNone())
			{
				AttachTarget = Pawn->FindComponentByTag<USceneComponent>(Spawn.AttachComponent);
			}

			if (Spawn.bUseAutonomousProxyComponent && BotaniCharacter && BotaniCharacter->IsLocallyControlled())
			{
				ActorToSpawn.AttachSocket = Spawn.AutonomousProxyAttachSocket;
				ActorToSpawn.AttachTransform = Spawn.AutonomousProxyAttachTransform;

				if (USceneComponent* SceneComponent = BotaniCharacter->FindComponentByTag<USceneComponent>(Spawn.AutonomousProxyAttachComponent))
				{
					AttachTarget = SceneComponent;
				}
				else
				{
					BOTANI_LOG(Warning, TEXT("Failed to find autonomous proxy component %s on %s"), *Spawn.AutonomousProxyAttachComponent.ToString(), *BotaniCharacter->GetName());
				}
			}

			Query.AttachTarget = AttachTarget;
			Query.ActorToSpawn = ActorToSpawn;
			Queries.Add(Query);
		}

		return Queries;
	}
};

/**
 * 
 */
UCLASS()
class BOTANIGAME_API UBotaniEquipmentDefinition : public UGameplayEquipmentDefinition
{GENERATED_UCLASS_BODY()};
