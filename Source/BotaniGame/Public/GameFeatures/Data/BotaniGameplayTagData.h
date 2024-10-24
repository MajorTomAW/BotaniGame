// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "BotaniGameplayTagData.generated.h"

//////////////////////////////////////////////////////////////////////////
/// Failure Responses

/**
 * UBotaniAbilityActivationFailureResponse
 */
UCLASS(Const, EditInlineNew, Abstract, CollapseCategories)
class UBotaniAbilityActivationFailureResponse : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnFailureResponse(const FGameplayTag& FailureTag) const PURE_VIRTUAL(UBotaniAbilityActivationFailureResponse::OnFailureResponse, );

protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = false, EditConditionHides))
	FName ResponseName;
#endif
};

/**
 * UActivationFailureResponse_DisplayMessage
 */
UCLASS(DisplayName = "Display Message")
class UActivationFailureResponse_DisplayMessage : public UBotaniAbilityActivationFailureResponse
{
	GENERATED_BODY()

public:
	UActivationFailureResponse_DisplayMessage();

public:
	/** The message to display when the ability fails to activate */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Activation Failure")
	FText RichFailureMessage;

	/** The amount of time to display the message */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Activation Failure")
	float DisplayTime = 2.0f;

	/** The priority of the message. Messages with higher priority will suppress messages with lower priority */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Activation Failure")
	int32 DisplayPriority = INDEX_NONE;
};

/**
 * UActivationFailureResponse_PlaySound
 */
UCLASS(DisplayName = "Play Sound")
class UActivationFailureResponse_PlaySound : public UBotaniAbilityActivationFailureResponse
{
	GENERATED_BODY()

public:
	UActivationFailureResponse_PlaySound();

public:
	/** The sound to play when the ability fails to activate */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Activation Failure")
	TSoftObjectPtr<class USoundBase> FailureSound;
};

/**
 * FBotaniAbilityActivationFailureTagMapping
 */
USTRUCT(BlueprintType)
struct FBotaniAbilityActivationFailureTagMapping
{
	GENERATED_BODY()

public:
	/** The tag that this container relationship is about. Single tag, but abilities can have multiple of these */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Activation Failure", meta = (Categories = "Ability.ActivationFail"))
	FGameplayTagContainer FailureTags;
	
	/** List of failure responses mapped to tags */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Activation Failure", Instanced, meta = (TitleProperty = "ResponseName"))
	TArray<TObjectPtr<UBotaniAbilityActivationFailureResponse>> FailureResponses;
};








//////////////////////////////////////////////////////////////////////////
/// Gameplay Tag Data

/**
 * UBotaniGameplayTagData
 *
 * Non-mutable data asset that contains properties used to define gameplay tags.
 */
UCLASS(Const)
class BOTANIGAME_API UBotaniGameplayTagData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UBotaniGameplayTagData(const FObjectInitializer& ObjectInitializer);

protected:
	/** Tag that an ability can have to indicate that its failure won't trigger a failure response */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abliity Activation Failure")
	FGameplayTag AbilityFailureNoMessageTag;

	/** List of failure responses mapped to tags */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abliity Activation Failure", meta = (TitleProperty = "FailureTags"))
	TArray<FBotaniAbilityActivationFailureTagMapping> FailureResponses;
};
