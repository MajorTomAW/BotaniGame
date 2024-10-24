// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BotaniCharacterModifier.generated.h"


/**
 * UBotaniCharacterModifier
 *
 * A single modifier that can modify the character that will be spawned.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, meta = (DisplayName = "Character Modifier"))
class BOTANIAI_API UBotaniCharacterModifier : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Called when the character has been spawned and the modifier should be applied to it. */
	virtual void ApplyToCharacter(ACharacter* Character) const PURE_VIRTUAL(UBotaniCharacterModifier::ApplyToCharacter, );
	
protected:
#if WITH_EDITORONLY_DATA
	/** The editor-friendly name of the modifier */
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "false", EditConditionHides))
	FString EditorFriendlyName;
#endif

#if WITH_EDITOR
	//~ Begin UObject Interface
	virtual void PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext) override;
	//~ End UObject Interface

	/** Gets the editor friendly name of the modifier */
	UFUNCTION(BlueprintNativeEvent, Category = "Editor")
	FString GetEditorFriendlyName() const;

	bool bHasBlueprintFriendlyName = false;
#endif
};
