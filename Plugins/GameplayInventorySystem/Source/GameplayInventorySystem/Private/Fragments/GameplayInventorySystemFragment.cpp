// Copyright © 2024 MajorT. All rights reserved.


#include "Fragments/GameplayInventorySystemFragment.h"

#include "Requirements/GameplayInventoryRequirement.h"

#if WITH_EDITOR
#include "UObject/ObjectSaveContext.h"
#include "Misc/DataValidation.h"
#endif


UGameplayInventorySystemFragment::UGameplayInventorySystemFragment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	EditorFriendlyName = GetEditorFriendlyName();
#endif
}

bool UGameplayInventorySystemFragment::CanApplyInventoryFragment(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, FGameplayTagContainer* OutBlockingTags) const
{
	bool bResult = true;

	if (!K2_CanApplyInventoryFragment(Context, Handle, *OutBlockingTags))
	{
		bResult = false;
	}

	for (const UGameplayInventoryRequirement* Requirement : FragmentRequirements)
	{
		if (Requirement == nullptr)
		{
			continue;
		}

		/*if (!Requirement->CanInventoryItemPerformAction(, Context)) @TODO: Item requirement should take in an handle instead of a spec
		{
			bResult = false;
		}*/
	}

	return bResult;
}
bool UGameplayInventorySystemFragment::K2_CanApplyInventoryFragment_Implementation( const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, FGameplayTagContainer& OutBlockingTags) const
{
	return true;
}

void UGameplayInventorySystemFragment::OnInstanceCreated(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, class UObject* Instance) const
{
	K2_OnInstanceCreated(Context, Handle, Instance);
}
void UGameplayInventorySystemFragment::K2_OnInstanceCreated_Implementation(const FGameplayInventoryItemContext& Context,const FGameplayInventoryItemSpecHandle& Handle, class UObject* Instance) const
{
}

void UGameplayInventorySystemFragment::OnInstanceDestroyed(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, class UObject* Instance) const
{
	K2_OnInstanceDestroyed(Context, Handle, Instance);
}
void UGameplayInventorySystemFragment::K2_OnInstanceDestroyed_Implementation( const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, class UObject* Instance) const
{
}

#if WITH_EDITOR
FString UGameplayInventorySystemFragment::GetEditorFriendlyName_Implementation() const
{
	FString FriendlyName = FName::NameToDisplayString(GetNameSafe(GetClass()), false);
	FriendlyName.ReplaceInline(TEXT("Item"), TEXT(""));
	FriendlyName.ReplaceInline(TEXT("Fragment"), TEXT(""));
	FriendlyName.ReplaceInline(TEXT("_C"), TEXT(""));
	return FriendlyName;
}

void UGameplayInventorySystemFragment::PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext)
{
	UObject::PostSaveRoot(ObjectSaveContext);
	EditorFriendlyName = GetEditorFriendlyName();
}

void UGameplayInventorySystemFragment::PostCDOCompiled(const FPostCDOCompiledContext& Context)
{
	UObject::PostCDOCompiled(Context);
	EditorFriendlyName = GetEditorFriendlyName();
}

EDataValidationResult UGameplayInventorySystemFragment::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	for (const UGameplayInventoryRequirement* Requirement : FragmentRequirements)
	{
		if (Requirement == nullptr)
		{
			Context.AddError(NSLOCTEXT("ItemFragmentError", "ItemFragmentRequirementNull", "Item fragment requirement is null."));
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
		}
	}

	return Result;
}
#endif