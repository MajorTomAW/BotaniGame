// Copyright © 2024 Game Shifters Studio. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "GHDataDefinitions.h"
#include "GHInputProcessor.h"
#include "GHEditorSubsystem.generated.h"

/**
 * Gizmo Helper Editor Subsystem
 */
UCLASS()
class UGHEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
	
public:
	UGHEditorSubsystem();
	virtual ~UGHEditorSubsystem();
	
	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	void InitializeKeyActions();

	// Shared pointer to the input processor
	TSharedPtr<FGHInputProcessor> InputProcessor = nullptr;

	// List of all pressed keys
	TArray<FKey> PressedKeys;

	// Checker for mouse button down
	bool bIsMouseButtonDown;

	// Last-cached mouse hit location
	FVector LastMouseHitLocation;

	/** Checks whether a modifier key is pressed */
	bool IsModifierKeyPressed();

	/** Checks whether the viewport is focused */
	static bool IsUserFocusedViewport();
	static bool IsUserFocusedTextBlock();
	static bool IsViewportUnderCursor();
	static void GetSelectedActors(TArray<AActor*>& OutActors, TArray<AActor*>& OutChildActors);
	static void GetChildActors(const AActor* Actor, TArray<AActor*>& OutActors, bool bRecursive = true);

	/** Checks whether a key is pressed that is relevant for the gizmo helper */
	static bool IsActionKey(const FKey& Key);

protected:
	UFUNCTION()
	void OnKeyPressDown(FKey Key);

	UFUNCTION()
	void OnKeyPressUp(FKey Key);
	
	UFUNCTION()
	void OnMouseButtonDown(FKey Key);

	UFUNCTION()
	void OnMouseButtonUp(FKey Key);

private:
	// Called to get the bounding box of the selected actor
	static FBox GetBoundingBox(const AActor* RelevantActor, bool& bIsCustom);

	// Called to place a single object with virtual collision
	void PlaceVirtualObject(AActor* Actor, const FVector& TargetLocation, const FHitResult& HitResult) const;

	// Called to perform the viewport raycast
	bool PerformViewportRaycast(const FLevelEditorViewportClient* EditorViewportClient, FHitResult& HitResult, const TArray<AActor*>& ActorsToIgnore = TArray<AActor*>());
	static TArray<AActor*> GetActorsToIgnore(TArray<TObjectPtr<AActor>>& Actors);

	static bool UpdateGizmoToActorLocation(const AActor* Actor);
	static bool GetMouseHitLocation(FVector& Start, FVector& Direction);

	static void MoveActorsBehindCamera(TArray<TObjectPtr<AActor>>& Actors, const bool bExtendRange = false);
	static void MoveActorsBehindCamera(AActor* Actor, const bool bExtendRange = false);

	void PlaceAssetFromContentBrowser(bool bIsRespectingCollision);
	void PlaceAssetsInViewport(bool bIsRespectingCollision);

	EGizmoSelectionMode::Type CurrentSelectionMode;
	static EGizmoSelectionMode::Type GetSelectionMode(const TSharedPtr<SWidget>& Widget);

	UPROPERTY()
	TObjectPtr<AActor> LastSpawnedActor;
	TSharedPtr<FScopedTransaction> GizmoTransaction;
	TSharedPtr<FGizmoHelperVars> GizmoHelperVars;
};
