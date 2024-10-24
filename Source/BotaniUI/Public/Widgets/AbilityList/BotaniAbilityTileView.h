// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonTileView.h"
#include "GameplayAbilitySpec.h"
#include "BotaniAbilityTileView.generated.h"

UCLASS(NotBlueprintable, BlueprintType)
class UBotaniAbilityTileEntryData : public UObject
{
	GENERATED_BODY()
	friend class UBotaniAbilityTileView;

public:
	UBotaniAbilityTileEntryData();

	void Initialize(const FGameplayAbilitySpec& InAbilitySpec);

	/** The input tags for this ability. */
	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	FGameplayTagContainer InputTags;

protected:
	const FGameplayAbilitySpec* AbilitySpec;
	FGameplayAbilitySpecHandle Handle;
};

/**
 * UBotaniAbilityTileView
 *
 * A tile view that displays abilities.
 */
UCLASS()
class BOTANIUI_API UBotaniAbilityTileView : public UCommonTileView
{
	GENERATED_UCLASS_BODY()

public:
	/** Initializes the tile view with the given ability system component. */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void InitializeWithAbilitySystem(UAbilitySystemComponent* InAbilitySystemComponent);

	/** Called to uninitialize the tile view. */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void UninitializeAbilitySystem();

protected:
	virtual void AddInitialEntries();
	virtual void AddAbilitySpec(const FGameplayAbilitySpec& AbilitySpec);
	
	virtual void RegisterAbilitySystemEvents();
	virtual void UnregisterAbilitySystemEvents();
	
	virtual void OnGameplayTagChanged(const FGameplayTag GameplayTag, int32 Count);
	virtual void OnAbilityActivated(UGameplayAbility* Ability);
	virtual void OnAbilityEnded(UGameplayAbility* Ability);
	virtual void OnAbilitiesReplicated(const TArray<FGameplayAbilitySpec>& Abilities);

private:
	UBotaniAbilityTileEntryData* FindEntryDataByHandle(const FGameplayAbilitySpecHandle Handle) const;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
