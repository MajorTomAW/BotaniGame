// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/Object.h"
#include "GameplayEquipmentDefinition.generated.h"

class AGameplayEquipmentItem;

/**
 * FGameplayEquipmentActorToSpawn
 */
USTRUCT(BlueprintType)
struct GAMEPLAYINVENTORYSYSTEM_API FGameplayEquipmentActorToSpawn
{
	GENERATED_BODY()

	FGameplayEquipmentActorToSpawn()
	{
	}

public:
	/** The actor to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	TSoftClassPtr<AGameplayEquipmentItem> ActorToSpawn;

	/** The socket to attach the actor to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (AnimNotifyBoneName = "true"))
	FName AttachSocket;

	/** The transform to attach the actor with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FTransform AttachTransform;

public:
	bool IsValid() const
	{
		return !ActorToSpawn.IsNull();
	}

	bool operator==(const FGameplayEquipmentActorToSpawn& Other) const
	{
		return (AttachSocket == Other.AttachSocket) && (AttachTransform.Equals(Other.AttachTransform));
	}
};

/**
 * FGameplayEquipmentActorSpawnQuery
 *
 * A single query for spawning an equipment actor.
 */
struct GAMEPLAYINVENTORYSYSTEM_API FGameplayEquipmentActorSpawnQuery
{
public:
	FGameplayEquipmentActorToSpawn ActorToSpawn;
	TWeakObjectPtr<USceneComponent> AttachTarget;

public:
	bool operator==(const FGameplayEquipmentActorSpawnQuery& Other) const
	{
		return (Other.ActorToSpawn == ActorToSpawn) && (Other.AttachTarget == AttachTarget);
	}

	bool IsValid() const
	{
		return ActorToSpawn.IsValid() && AttachTarget.IsValid();
	}
};

/**
 * UGameplayEquipmentActorToSpawnRule
 */
UCLASS(EditInlineNew, Abstract, Blueprintable, BlueprintType, Const, CollapseCategories, MinimalAPI, meta = (DisplayName = "Spawn Rule"), HideCategories = ("Object"))
class UGameplayEquipmentActorToSpawnRule : public UObject
{
	GENERATED_BODY()

public:
	GAMEPLAYINVENTORYSYSTEM_API virtual TArray<FGameplayEquipmentActorSpawnQuery> GetActorSpawnQueries(const APawn* Pawn) const
	{
		return TArray<FGameplayEquipmentActorSpawnQuery>();
	}
};

/**
 * UEquipmentActorToSpawnRule_Simple
 */
UCLASS(meta = (DisplayName = "Spawn Rule - Default"))
class UEquipmentActorToSpawnRule_Simple : public UGameplayEquipmentActorToSpawnRule
{
	GENERATED_BODY()

public:
	/** Actors to spawn on the pawn when this is equipped */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment", meta = (TitleProperty = "ActorToSpawn"))
	TArray<FGameplayEquipmentActorToSpawn> ActorsToSpawn;

public:
	GAMEPLAYINVENTORYSYSTEM_API virtual TArray<FGameplayEquipmentActorSpawnQuery> GetActorSpawnQueries(const APawn* Pawn) const override
	{
		check(Pawn);

		USceneComponent* AttachTarget = Pawn->GetRootComponent();

		if (const ACharacter* Character = Cast<ACharacter>(Pawn))
		{
			AttachTarget = Character->GetMesh();
		}

		TArray<FGameplayEquipmentActorSpawnQuery> Queries;

		for (const auto& Spawn : ActorsToSpawn)
		{
			FGameplayEquipmentActorSpawnQuery Query;
			Query.ActorToSpawn = Spawn;
			Query.AttachTarget = AttachTarget;
			Queries.Add(Query);
		}

		return Queries;
	}
};

/**
 * UGameplayEquipmentDefinition
 *
 * A definition for an equipment item in the gameplay inventory system.
 */
UCLASS(MinimalAPI, Blueprintable, BlueprintType, Const, Config = Game, meta = (ShortTooltip = "A definition for an equipment item in the gameplay inventory system.", DisplayName = "Equipment Definition"))
class UGameplayEquipmentDefinition : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** The actor spawn rule to use when this equipment is equipped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Equipment", meta = (DisplayName = "Actor Spawn Rule"))
	TObjectPtr<UGameplayEquipmentActorToSpawnRule> ActorSpawnRule;

	/** The class of the equipment instance that is created when equipped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (DisplayName = "Override Equipment Instance"))
	TSubclassOf<class UGameplayEquipmentInstance> EquipmentInstanceClass;

	/** List of additional fragments to apply when the equipment is equipped */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Fragments", meta = (DisplayName = "Fragments", TitleProperty = EditorFriendlyName, ShowOnlyInnerProperties))
	TArray<TObjectPtr<class UGameplayEquipmentFragment>> EquipmentFragments;

public:
	GAMEPLAYINVENTORYSYSTEM_API virtual UClass* GetInstanceClass() const;
	
protected:
#if WITH_EDITOR
	//~ Begin UObject Interface
	GAMEPLAYINVENTORYSYSTEM_API virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	GAMEPLAYINVENTORYSYSTEM_API virtual void PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext) override;
	//~ End UObject Interface
#endif

protected:
	UPROPERTY(Config)
	TSubclassOf<UGameplayEquipmentInstance> DefaultInstanceClass;
};
