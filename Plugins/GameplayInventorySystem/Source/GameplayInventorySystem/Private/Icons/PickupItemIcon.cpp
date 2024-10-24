// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Icons/PickupItemIcon.h"

#if WITH_EDITOR
#include "UObject/ObjectSaveContext.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(PickupItemIcon)

UPickupItemIcon::UPickupItemIcon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	EditorFriendlyName = GetEditorFriendlyName();
#endif
}

void UPickupItemIcon::ApplyPickupIcon(class APickupProxyActor* InProxy, const class UGameplayInventoryItemDefinition* InItemDef)
{
	K2_ApplyPickupIcon(InProxy, InItemDef);
}

#if WITH_EDITOR
void UPickupItemIcon::PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext)
{
	EditorFriendlyName = GetEditorFriendlyName();
	
	UObject::PostSaveRoot(ObjectSaveContext);
}

void UPickupItemIcon::PostCDOCompiled(const FPostCDOCompiledContext& Context)
{
	EditorFriendlyName = GetEditorFriendlyName();
	
	UObject::PostCDOCompiled(Context);
}
#endif

#if WITH_EDITORONLY_DATA
FString UPickupItemIcon::GetEditorFriendlyName_Implementation() const
{
	return TEXT("Unnamed Icon");
}
#endif




UPickupIconModifier::UPickupIconModifier(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPickupIconModifier::ApplyModifier(
	UPickupItemIcon* Outer, USceneComponent* SpawnedIcon, APickupProxyActor* PickupProxy, const UGameplayInventoryItemDefinition* InItemDef)
{
	K2_ApplyModifier(Outer, SpawnedIcon, PickupProxy, InItemDef);
}

