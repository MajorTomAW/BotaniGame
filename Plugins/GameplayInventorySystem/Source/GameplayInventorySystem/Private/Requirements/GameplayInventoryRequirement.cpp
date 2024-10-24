// Copyright © 2024 MajorT. All rights reserved.


#include "Requirements/GameplayInventoryRequirement.h"
#include "Definitions/GameplayInventoryItemDefinition.h"

#if WITH_EDITOR
#include "UObject/ObjectSaveContext.h"
#endif

UGameplayInventoryRequirement::UGameplayInventoryRequirement()
{
/*#if WITH_EDITORONLY_DATA
	EditorFriendlyName = GetEditorFriendlyName();
#endif*/
}

bool UGameplayInventoryRequirement::CanInventoryItemPerformAction(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& Context) const
{
	bool bResult = true;

	if (!CanInventoryItemPerformActionBP(ItemSpec, Context))
	{
		bResult = false;
	}

	return bResult;
}

bool UGameplayInventoryRequirement::CanInventoryItemPerformActionBP_Implementation(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& Context) const
{
	return true;
}

UGameplayInventoryItemDefinition* UGameplayInventoryRequirement::GetOwningItemDef( UGameplayInventoryItemDefinition* Class) const
{
	UGameplayInventoryItemDefinition* Def = GetOwningItem<UGameplayInventoryItemDefinition>();
	if (Def->IsA(Class->GetClass()))
	{
		return Def;
	}

	return nullptr;
}

/*
#if WITH_EDITOR
void UGameplayInventoryRequirement::PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext)
{
	UObject::PostSaveRoot(ObjectSaveContext);
	
	//EditorFriendlyName = GetEditorFriendlyName(); @TODO: Re-enable this line?!
}
#endif

#if WITH_EDITORONLY_DATA
FString UGameplayInventoryRequirement::GetEditorFriendlyName_Implementation() const
{
	FString FriendlyName = FName::NameToDisplayString(GetNameSafe(GetClass()), false);
	FriendlyName.ReplaceInline(TEXT("Item"), TEXT(""));
	FriendlyName.ReplaceInline(TEXT("Requirement"), TEXT(""));
	FriendlyName.ReplaceInline(TEXT("Gameplay"), TEXT(""));
	FriendlyName.ReplaceInline(TEXT("Inventory"), TEXT(""));
	return FriendlyName;
}
#endif
*/
