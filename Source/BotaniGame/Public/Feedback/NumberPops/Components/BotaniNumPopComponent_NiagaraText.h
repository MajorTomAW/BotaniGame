// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniNumberPopComponent.h"
#include "BotaniNumPopComponent_NiagaraText.generated.h"

class UNiagaraComponent;
/**
 *
 * UBotaniNumPopComponent_NiagaraText
 *
 * A component that can be added to a controller to allow for the display of number pops using Niagara text.
 */
UCLASS(Blueprintable)
class BOTANIGAME_API UBotaniNumPopComponent_NiagaraText : public UBotaniNumberPopComponent
{
	GENERATED_UCLASS_BODY()

public:
	/** Static function to find a number pop component on an actor */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Number Pop", meta = (DefaultToSelf = "Actor"))
	static UBotaniNumPopComponent_NiagaraText* FindNumPopComponent(const AActor* Actor);

public:
	//~ Begin UBotaniNumberPopComponent interface
	virtual void AddNumberPop(const FBotaniNumPopRequest& NewRequest) override;
	//~ End of UBotaniNumberPopComponent interface
	
protected:
	/** List of cached number pop requests */
	TArray<int32> NumberArray;

	/** Style asset to apply to the incoming number pops */
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<class UBotaniNumPopStyle_Niagara> Style;
	
private:
	/** The Niagara component used to display the damage */
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> NiagaraComp;
};
