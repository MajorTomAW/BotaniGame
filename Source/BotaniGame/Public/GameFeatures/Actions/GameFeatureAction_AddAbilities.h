// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameplayAbilitySpecHandle.h"
#include "AbilitySystem/BotaniAbilitySet.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFeatureAction_AddAbilities.generated.h"

class UGameplayAbility;
class UAttributeSet;

class UBotaniAbilitySet;

//////////////////////////////////////////////////////////////////////////
/// FBotaniAbilityGrant
//////////////////////////////////////////////////////////////////////////

/**
 * FBotaniAbilityGrant
 *
 * Structure representing an ability to grant to an actor.
 */
USTRUCT(BlueprintType)
struct FBotaniAbilityGrant
{
	GENERATED_BODY()

public:
	/** Type of ability to grant */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Client,Server"))
	TSoftClassPtr<UGameplayAbility> AbilityType;
};

//////////////////////////////////////////////////////////////////////////
/// FBotaniAttributeSetGrant
//////////////////////////////////////////////////////////////////////////

/**
 * FBotaniAttributeSetGrant
 *
 * Structure representing an attribute set to grant to an actor.
 */
USTRUCT(BlueprintType)
struct FBotaniAttributeSetGrant
{
	GENERATED_BODY()

public:
	/** Attribute set to grant */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Client,Server"))
	TSoftClassPtr<UAttributeSet> AttributeSetType;

	/** Data table referent to initialize the attributes with, if any (can be left unset) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Client,Server"))
	TSoftObjectPtr<UDataTable> InitializationData;
};

//////////////////////////////////////////////////////////////////////////
/// FGameFeatureAbilitiesEntry
//////////////////////////////////////////////////////////////////////////

/**
 * FGameFeatureAbilitiesEntry
 *
 * Structure representing a set of abilities and attribute sets to grant to actors of a specified class.
 */
USTRUCT(BlueprintType)
struct FGameFeatureAbilitiesEntry
{
	GENERATED_BODY()

public:
	/** The base actor class to add to */
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TSoftClassPtr<AActor> ActorClass;

	/** List of abilities to grant to actors of the specified class */
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<FBotaniAbilityGrant> GrantedAbilities;

	/** List of attribute sets to grant to actors of the specified class */
	UPROPERTY(EditAnywhere, Category = "Attributes")
	TArray<FBotaniAttributeSetGrant> GrantedAttributes;

	/** List of ability sets to grant to actors of the specified class */
	UPROPERTY(EditAnywhere, Category = "Attributes", meta = (AssetBundles = "Client,Server"))
	TArray<TSoftObjectPtr<const UBotaniAbilitySet>> GrantedAbilitySets;
};


//////////////////////////////////////////////////////////////////////////
/// UGameFeatureAction_AddAbilities
//////////////////////////////////////////////////////////////////////////

/**
 * UGameFeatureAction_AddAbilities
 *
 * Game Feature Action responsible for adding abilities (and attribute sets) to actors of a specified type.
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Abilities"))
class UGameFeatureAction_AddAbilities final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	//~ End UGameFeatureAction interface

	//~ Begin UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject interface

protected:
	/** List of abilities to grant to actors */
	UPROPERTY(EditAnywhere, Category = "Abilities", meta = (TitleProperty = "ActorClass", ShowOnlyInnerProperties))
	TArray<FGameFeatureAbilitiesEntry> AbilitiesList;

private:
	struct FActorExtensions
	{
		TArray<FGameplayAbilitySpecHandle> Abilities;
		TArray<UAttributeSet*> Attributes;
		TArray<FBotaniAbilitySet_GrantedHandles> AbilitySetHandles;
	};

	struct FPerContextData
	{
		TMap<AActor*, FActorExtensions> ActiveExtensions;
		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;
	};
	typedef TMap<FGameFeatureStateChangeContext, FPerContextData> FContextData;
	FContextData ContextData;

private:
	//~ Begin UGameFeatureAction_WorldActionBase interface
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
	//~ End UGameFeatureAction_WorldActionBase interface

private:
	void Reset(FPerContextData& ActiveData);
	void HandleActorExtension(AActor* Actor, FName EventName, int32 EntryIndex, FGameFeatureStateChangeContext ChangeContext);
	void AddActorAbilities(AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry, FPerContextData& ActiveData);
	void RemoveActorAbilities(AActor* Actor, FPerContextData& ActiveData);

	template <class Type>
	Type* FindOrAddComponentForActor(AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry, FPerContextData& ActiveData)
	{
		return Cast<Type>(FindOrAddComponentForActor(Type::StaticClass(), Actor, AbilitiesEntry, ActiveData));
	}

	UActorComponent* FindOrAddComponentForActor(UClass* ComponentType, AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry, FPerContextData& ActiveData);
};
