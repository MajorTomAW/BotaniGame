// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "GameplayEffectTypes.h"
#include "BotaniGameplayEffectContext.generated.h"

class IBotaniAbilitySourceInterface;

USTRUCT()
struct FBotaniGameplayEffectContext : public FGameplayEffectContext 
{
	GENERATED_BODY()

public:
	FBotaniGameplayEffectContext();
	FBotaniGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser);

	/** Returns the wrapped FBotaniGameplayEffectContext from the handle, or nullptr if it doesn't exist or is the wrong type */
	static BOTANIGAME_API FBotaniGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);

	/** Sets the object used as the ability source */
	BOTANIGAME_API void SetAbilitySource(const IBotaniAbilitySourceInterface* InObject, float InSourceLevel);

	/** Returns the ability source interface associated with the source object. Only valid on the authority. */
	BOTANIGAME_API const IBotaniAbilitySourceInterface* GetAbilitySource() const;

	/** Returns the physical material from the hit result if there is one */
	BOTANIGAME_API const UPhysicalMaterial* GetPhysicalMaterial() const;

	//~ Begin FGameplayEffectContext Interface
	virtual FGameplayEffectContext* Duplicate() const override;
	virtual UScriptStruct* GetScriptStruct() const override;
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
	//~ End FGameplayEffectContext Interface

public:
	/** Id to allow the identification of multiple bullets that were part of the same cartridge */
	UPROPERTY()
	int32 CartridgeID = INDEX_NONE;

protected:
	/** Ability Source object (should implement IBotaniAbilitySourceInterface). NOT replicated. */
	UPROPERTY()
	TWeakObjectPtr<const UObject> AbilitySourceObject;
};

template<>
struct TStructOpsTypeTraits<FBotaniGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FBotaniGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true,
	};
};
