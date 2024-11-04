// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BotaniReticleWidget.generated.h"

/**
 * UBotaniReticleWidget
 *
 * A widget that displays a reticle for a weapon instance.
 */
UCLASS(Abstract)
class BOTANIUI_API UBotaniReticleWidget : public UCommonUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	/** Called when the weapon instance is initialized and both WeaponEquipmentInstance and InventoryInstance are set */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon", meta = (DisplayName = "On Weapon Initialized"))
	void OnWeaponInitialized_BP();

	/** Called when the widget should fade out */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon", meta = (DisplayName = "On Fade Out"))
	void OnFadeOut_BP();

	/** Called when the widget should fade in */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon", meta = (DisplayName = "On Fade In"))
	void OnFadeIn_BP();

	/** Called to initialize the reticle from a weapon instance */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void InitializeFromWeapon(class UBotaniWeaponEquipmentInstance* InWeaponInstance);

	/** Called to restart the fade timer */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void RestartFadeTimer();

	/** Returns the current weapon's diametrical spread in angle (in degrees) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	float ComputeSpreadAngle() const;

public:
	//~ Begin UUserWidget interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~ End UUserWidget interface
	

protected:
	/** The duration in seconds to stay visible after a widget update */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reticle")
	float FadeDuration;
	
	/** The weapon instance associated with this reticle */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class UBotaniWeaponEquipmentInstance> WeaponEquipmentInstance;

	/** The inventory item instance associated with this reticle */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class UBotaniItemInstance> InventoryInstance;

private:
	FTimerHandle FadeTimerHandle;
};
