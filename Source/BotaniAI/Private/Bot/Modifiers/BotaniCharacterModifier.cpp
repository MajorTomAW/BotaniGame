// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Bot/Modifiers/BotaniCharacterModifier.h"

#if WITH_EDITOR
#include "UObject/ObjectSaveContext.h"
#endif

UBotaniCharacterModifier::UBotaniCharacterModifier(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	auto ImplementedInBlueprint = [](const UFunction* Func)
	{
		return Func && ensure(Func->GetOuter()) &&
			Func->GetOuter()->IsA(UBlueprintGeneratedClass::StaticClass());
	};

	{
		static FName FuncName = FName("GetEditorFriendlyName");
		const UFunction* Func = FindFunction(FuncName);

#if WITH_EDITORONLY_DATA
		bHasBlueprintFriendlyName = ImplementedInBlueprint(Func);
#endif
	}
	
#if WITH_EDITORONLY_DATA
	if (bHasBlueprintFriendlyName)
	{
		EditorFriendlyName = GetEditorFriendlyName();	
	}
#endif
}

#if WITH_EDITOR
void UBotaniCharacterModifier::PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext)
{
	UObject::PostSaveRoot(ObjectSaveContext);

	if (bHasBlueprintFriendlyName)
	{
		EditorFriendlyName = GetEditorFriendlyName();
	}
}

FString UBotaniCharacterModifier::GetEditorFriendlyName_Implementation() const
{
	return TEXT("Character Modifier");
}
#endif
