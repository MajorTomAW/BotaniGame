// Copyright © 2024 MajorT. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Input/Reply.h"
#include "PlaysetRootActor.generated.h"

/**
 * APlaysetRootActor
 *
 * An actor class.
 */
UCLASS(HideCategories = (Input, Actor, Replication, Rendering, HLOD, Cooking, Physics, Navigation, VirtualTexture, Mobile, Collision, DataLayers, LevelInstance))
class GAMEPLAYSETS_API APlaysetRootActor : public AActor
{
	GENERATED_UCLASS_BODY()

public:
#if WITH_EDITORONLY_DATA
	/** The associated playset to this actor */
	UPROPERTY(VisibleDefaultsOnly, Category = "Playset", meta = (DisplayName = "Associated Playset"))
	TObjectPtr<class UPlayset> Playset;
#endif

#if WITH_EDITOR
	/** Updates the associated playset with the current version of the actor. */
	UFUNCTION(CallInEditor, Category = "Playset")
	void UpdateAssociatedPlayset();
#endif

protected:
	//~ Begin AActor Interface
	virtual void Destroyed() override;
	//~ End AActor Interface
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USceneComponent> SceneRootComponent;
	
private:
	
};
