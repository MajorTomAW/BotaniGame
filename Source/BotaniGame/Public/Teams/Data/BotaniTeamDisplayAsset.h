// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BotaniTeamDisplayAsset.generated.h"

/**
 * UBotaniTeamDisplayAsset
 *
 * Non-mutable data asset that represents the display information for team definitions (e.g., colors, display names, textures, etc...)
 */
UCLASS(BlueprintType, Const)
class BOTANIGAME_API UBotaniTeamDisplayAsset : public UDataAsset
{
	GENERATED_UCLASS_BODY()

public:
	/** Short name for the team */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team")
	FText TeamShortName;
	
	/** Scalar parameters to apply to materials */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style")
	TMap<FName, float> ScalarParameters;

	/** Color parameters to apply to materials */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style")
	TMap<FName, FLinearColor> ColorParameters;

	/** Texture parameters to apply to materials */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style")
	TMap<FName, TSoftObjectPtr<UTexture>> TextureParameters;

public:
	/** Applies the display asset to a material */
	UFUNCTION(BlueprintCallable, Category = "Botani|Teams")
	void ApplyToMaterial(UMaterialInstanceDynamic* Material);

	/** Applies the display asset to a mesh component */
	UFUNCTION(BlueprintCallable, Category = "Botani|Teams")
	void ApplyToMeshComponent(UMeshComponent* MeshComponent);

	/** Applies the display asset to a Niagara component */
	UFUNCTION(BlueprintCallable, Category = "Botani|Teams")
	void ApplyToNiagaraComponent(class UNiagaraComponent* NiagaraComponent);

	/** Applies the display asset to an actor */
	UFUNCTION(BlueprintCallable, Category = "Botani|Teams", meta = (DefaultToSelf = "TargetActor"))
	void ApplyToActor(AActor* TargetActor, bool bIncludeChildActors = true);

protected:
	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface
};
