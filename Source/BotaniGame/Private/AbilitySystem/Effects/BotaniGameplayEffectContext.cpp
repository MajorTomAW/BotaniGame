// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/Effects/BotaniGameplayEffectContext.h"

#include "AbilitySystem/BotaniAbilitySourceInterface.h"
#include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"
#include "Serialization/GameplayEffectContextNetSerializer.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniGameplayEffectContext)


FBotaniGameplayEffectContext::FBotaniGameplayEffectContext()
	: FGameplayEffectContext()
{
}

FBotaniGameplayEffectContext::FBotaniGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
	: FGameplayEffectContext(InInstigator, InEffectCauser)
{
}

FBotaniGameplayEffectContext* FBotaniGameplayEffectContext::ExtractEffectContext(FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) &&
		(BaseEffectContext->GetScriptStruct()->IsChildOf(StaticStruct())))
	{
		return static_cast<FBotaniGameplayEffectContext*>(BaseEffectContext);
	}

	return nullptr;
}

void FBotaniGameplayEffectContext::SetAbilitySource(const IBotaniAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel; @TODO: Implement SourceLevel
}

const IBotaniAbilitySourceInterface* FBotaniGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IBotaniAbilitySourceInterface>(AbilitySourceObject.Get());
}

const UPhysicalMaterial* FBotaniGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* Hit = GetHitResult())
	{
		return Hit->PhysMaterial.Get();
	}

	return nullptr;
}

FGameplayEffectContext* FBotaniGameplayEffectContext::Duplicate() const
{
	FBotaniGameplayEffectContext* NewContext = new FBotaniGameplayEffectContext();
	*NewContext = *this;

	if (GetHitResult())
	{
		// Performs a deep copy of the hit result
		NewContext->AddHitResult(*GetHitResult(), true);
	}

	return NewContext;
}

UScriptStruct* FBotaniGameplayEffectContext::GetScriptStruct() const
{
	return FBotaniGameplayEffectContext::StaticStruct();
}

bool FBotaniGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// CartridgeID
	
	return true;
}


#if UE_WITH_IRIS
namespace UE::Net
{
	// Forward to FGameplayEffectContextNetSerializer
	// Note: If FBotaniGameplayEffectContext::NetSerialize() is modified, a custom NetSerializer must be implemented as the current fallback will no longer be sufficient.
	UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES(FBotaniGameplayEffectContext, FGameplayEffectContextNetSerializer);
}
#endif