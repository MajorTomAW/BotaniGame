// Copyright © 2024 Game Shifters Studio. All rights reserved.


#include "GHEditorSubsystem.h"

#include "ContentBrowserModule.h"
#include "Engine/Selection.h"
#include "LevelEditorViewport.h"
#include "EditorModeManager.h"
#include "GHDeveloperSettings.h"
#include "Editor.h"
#include "GameFramework/PlayerController.h"
#include "Licensing/GHLicenseManager.h"
#include "EditorViewportClient.h"
#include "GHDataDefinitions.h"
#include "GizmoHelperTool.h"
#include "IContentBrowserSingleton.h"
#include "ActorFactories/ActorFactory.h"
#include "Factories/AssetFactoryInterface.h"
#include "Subsystems/PlacementSubsystem.h"

#define LOCTEXT_NAMESPACE "GizmoHelperTool"

UGHEditorSubsystem::UGHEditorSubsystem()
{
	bIsMouseButtonDown = false;
	CurrentSelectionMode = EGizmoSelectionMode::Viewport;
	LastMouseHitLocation = FVector::ZeroVector;
}

UGHEditorSubsystem::~UGHEditorSubsystem()
{
	if (InputProcessor.IsValid())
	{
		InputProcessor.Reset();
	}
}

void UGHEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (FGHLicenseManager::bIsReady)
	{
		InitializeKeyActions();
	}
	else
	{
		FGHLicenseManager::WaitLicenseManagerReadyDelegate.BindLambda([this]()
		{
			InitializeKeyActions();
		});
	}
}

void UGHEditorSubsystem::InitializeKeyActions()
{
	if (!FGHLicenseManager::bIsValidLicenseKey)
	{
		GIZMO_LOG(Error, TEXT("Gizmo Helper Tool is not licensed. Gizmo Helper Tool will not work."));
		return;
	}

	// Create a custom input processor and register it to the slate application, then bind input events that are handled via this subsystem
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication& SlateApp = FSlateApplication::Get();
		InputProcessor = MakeShared<FGHInputProcessor>();
		
		SlateApp.RegisterInputPreProcessor(InputProcessor);
		
		InputProcessor->OnKeyPressedDownDelegate.AddDynamic(this, &ThisClass::OnKeyPressDown);
		InputProcessor->OnKeyPressedUpDelegate.AddDynamic(this, &ThisClass::OnKeyPressUp);
		InputProcessor->OnMouseButtonDownDelegate.AddDynamic(this, &ThisClass::OnMouseButtonDown);
		InputProcessor->OnMouseButtonUpDelegate.AddDynamic(this, &ThisClass::OnMouseButtonUp);

		

		SlateApp.OnApplicationActivationStateChanged().AddLambda([this](const bool bIsActive)
		{
			if (bIsActive)
			{
				// Nothing to do here
				return;
			}

#if IS_GIZMO_DEBUG
			UE_LOG(LogGizmoDebug, Display, TEXT("Application is not active. Resetting pressed keys. [Num: %d]"), PressedKeys.Num());
#endif
			
			// If the application is not active, reset the pressed keys
			PressedKeys.Empty();
		});
	}
	else
	{
		GIZMO_LOG(Error, TEXT("Slate application is not initialized. Gizmo Helper Tool will not work."));
	}

	GIZMO_LOG(Log, TEXT("Gizmo Helper Tool successfully initialized."));
}

void UGHEditorSubsystem::Deinitialize()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);	
	}
	
	Super::Deinitialize();
}

void UGHEditorSubsystem::GetSelectedActors(TArray<AActor*>& OutActors, TArray<AActor*>& OutChildActors)
{
	// Get all selected actors
	const USelection* SelectedActors = GEditor->GetSelectedActors();
	TArray<AActor*> Actors;
	Actors.Reserve(SelectedActors->Num());

	for (int i = 0; i < SelectedActors->Num(); i++)
	{
		AActor* SelectedActor = Cast<AActor>(SelectedActors->GetSelectedObject(i));
		if (SelectedActors == nullptr)
		{
			continue;
		}
		Actors.AddUnique(SelectedActor);
		GetChildActors(SelectedActor, OutChildActors, true);
	}

	OutActors = Actors;

	// Filter out child actors since we don't want to move them individually
	for (AActor* SelectedActor : Actors)
	{
		if (!Actors.Contains(SelectedActor->GetAttachParentActor()))
		{
			continue;
		}

		OutActors.Remove(SelectedActor);
		OutChildActors.Add(SelectedActor);
	}

	for (AActor* SelectedActor : Actors)
	{
		if (OutChildActors.Contains(SelectedActor))
		{
			OutChildActors.Remove(SelectedActor);
		}
	}

	Algo::Reverse(OutActors);
}

void UGHEditorSubsystem::GetChildActors(const AActor* Actor, TArray<AActor*>& OutActors, bool bRecursive)
{
	Actor->GetAttachedActors(OutActors, false,bRecursive);
}


FViewportCursorLocation GetViewportCursorLocation()
{
	FLevelEditorViewportClient* ViewportClient = GCurrentLevelEditingViewportClient;
	const FViewport* Viewport = ViewportClient->Viewport;
	
	const int32 ViewportInteractX = Viewport->GetMouseX();
	const int32 ViewportInteractY = Viewport->GetMouseY();
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		Viewport,
		ViewportClient->GetScene(),
		ViewportClient->EngineShowFlags).SetRealtimeUpdate(ViewportClient->IsRealtime()));
	const FSceneView* SceneView = ViewportClient->CalcSceneView(&ViewFamily);

	return FViewportCursorLocation(SceneView, ViewportClient, ViewportInteractX, ViewportInteractY);
}

void UGHEditorSubsystem::OnKeyPressDown(FKey Key)
{
	// Register pressed keys
	PressedKeys.Add(Key);
	
	// If any modifier key is pressed, stop because of copy and paste
	if (
		IsModifierKeyPressed() ||
		!IsActionKey(Key) ||
		bIsMouseButtonDown || 
		!IsViewportUnderCursor() ||
		IsUserFocusedTextBlock() ||
		!FGHLicenseManager::bIsValidLicenseKey
	)
	{
		return;
	}

	// Start a scoped transaction, grouping all changes
	if (!GEditor->IsTransactionActive())
	{
		FText TransactionName = CurrentSelectionMode == EGizmoSelectionMode::ContentBrowser ? FText::FromString(TEXT("From Content Browser")) : FText::FromString(TEXT("In Viewport"));
		TransactionName = FText::Format(LOCTEXT("GizmoHelperTool: ", "Move Actors: {0}"), TransactionName);
		GizmoTransaction = MakeShared<FScopedTransaction>(TransactionName);
	}

	const bool bMovingWithCollision = Key == UGHDeveloperSettings::Get()->MoveToMouseKey;

	switch (CurrentSelectionMode)
	{
	case EGizmoSelectionMode::ContentBrowser:
		{
			PlaceAssetFromContentBrowser(bMovingWithCollision);
			break;
		}
	case EGizmoSelectionMode::Viewport:
		{
			PlaceAssetsInViewport(bMovingWithCollision);
			break;
		}
	default:
		{
#if IS_GIZMO_DEBUG
			GIZMO_LOG(Error, TEXT("Invalid selection mode. Falling back to viewport selection mode."));
#endif
			CurrentSelectionMode = EGizmoSelectionMode::Viewport;
			break;
		}
	}
	

	// Refresh viewport (to fix clicking collision issues)
	FLevelEditorViewportClient* ViewportClient = GCurrentLevelEditingViewportClient;
	ViewportClient->Invalidate(true, true);
	ViewportClient->Viewport->Invalidate();
}

void UGHEditorSubsystem::OnKeyPressUp(FKey Key)
{
	// Unregister pressed keys
	PressedKeys.Remove(Key);
	
	// Check if the key is an action key to make sure we don't end the transaction for other keys
	if (!IsActionKey(Key))
	{
		return;
	}

	if (LastSpawnedActor != nullptr)
	{
		// Deselect all actors
		TArray<AActor*> Actors, ChildActors;
		GetSelectedActors(Actors, ChildActors);
		for (AActor* Actor : Actors)
		{
			GEditor->GetSelectedActors()->Deselect(Actor);
		}

		// Select the last spawned actor
		GEditor->SelectActor(LastSpawnedActor, true, true);
		LastSpawnedActor = nullptr;
	}
	
	// End the transaction
	if (GizmoTransaction.IsValid())
	{
		GizmoTransaction.Reset();
	}

	// Reset gizmo helper vars
	if (GizmoHelperVars.IsValid())
	{
		GizmoHelperVars.Reset();
	}
	
	bIsMouseButtonDown = false;
}

void UGHEditorSubsystem::OnMouseButtonDown(FKey Key)
{
	if (IsActionKey(Key))
	{
		bIsMouseButtonDown = true;
	}
}

void UGHEditorSubsystem::OnMouseButtonUp(FKey Key)
{
	const TSharedPtr<SWidget> FocusedWidget = FSlateApplication::Get().GetUserFocusedWidget(0);

	if (!FocusedWidget.IsValid())
	{
		return;
	}

	
	if (Key == EKeys::LeftMouseButton)
	{
		CurrentSelectionMode = GetSelectionMode(FocusedWidget);

#if IS_GIZMO_DEBUG
		UE_LOG(LogGizmoDebug, Display, TEXT("Selection mode changed to: %d"), static_cast<uint8>(CurrentSelectionMode));
#endif
	}
}

bool UGHEditorSubsystem::IsModifierKeyPressed()
{
	// Check if we have any keys pressed
	if (PressedKeys.IsEmpty())
	{
		return false;
	}
	
	// Returns true if any of the registered pressed keys is a modifier, otherwise returns false
	for (const FKey& Key : PressedKeys)
	{
		if (Key.IsModifierKey())
		{
			return true;
		}
	}
	return false;
}

bool UGHEditorSubsystem::IsUserFocusedViewport()
{
	// If user not focused on the viewport, stop.
	const TSharedPtr<SWidget> FocusedWidget = FSlateApplication::Get().GetUserFocusedWidget(0);


	if (FocusedWidget == nullptr)
	{
		return false;
	}


	if (FocusedWidget.IsValid() ? FocusedWidget->GetType() == GH_VIEWPORT : false)
	{
		return true;
	}
	
	return IsViewportUnderCursor();
}

bool UGHEditorSubsystem::IsUserFocusedTextBlock()
{
	// If user not focused on the viewport, stop.
	const TSharedPtr<SWidget> FocusedWidget = FSlateApplication::Get().GetUserFocusedWidget(0);
	return FocusedWidget->GetType().ToString().Contains(TEXT("Text"));
}

bool UGHEditorSubsystem::IsViewportUnderCursor()
{
	FSlateApplication& SlateApp = FSlateApplication::Get();
	const FVector2D CursorPos = SlateApp.GetCursorPos();
	FWidgetPath WidgetPath = SlateApp.LocateWindowUnderMouse(CursorPos, SlateApp.GetInteractiveTopLevelWindows());

	TArray<FName> WidgetTypes;
	for (int i = 0; i < WidgetPath.Widgets.Num(); i++)
	{
		WidgetTypes.Add(WidgetPath.Widgets[i].Widget->GetType());
	}

	bool bIsViewport = WidgetTypes.Contains(GH_VIEWPORT);

	if (bIsViewport)
	{
		bIsViewport = !WidgetTypes.Contains("SContentBrowser");
	}
	
	return bIsViewport;
}

bool UGHEditorSubsystem::IsActionKey(const FKey& Key)
{
	return UGHDeveloperSettings::Get()->GetAllInputKeys().Contains(Key);
}

bool UGHEditorSubsystem::UpdateGizmoToActorLocation(const AActor* Actor)
{
	// Check if the actor is valid
	if (Actor == nullptr)
	{
		return false;
	}

	const FLevelEditorViewportClient* ViewportClient = GCurrentLevelEditingViewportClient;

	// Check if the viewport client is valid
	if (ViewportClient == nullptr)
	{
		return false;
	}

	FEditorModeTools* ModeTools = ViewportClient->GetModeTools();
	ModeTools->SetPivotLocation(Actor->GetActorLocation(), false);
	return true;
}

bool UGHEditorSubsystem::GetMouseHitLocation(FVector& Start, FVector& Direction)
{
	// Logic for converting the mouse location in the editor to world location and direction
	Start = Direction = FVector::ZeroVector;

	FLevelEditorViewportClient* ViewportClient = GCurrentLevelEditingViewportClient;

	// Check if the viewport client is valid and not tracking (RMB looking)
	if (ViewportClient ? ViewportClient->IsTracking() : true)
	{
		return false;
	}

	const FViewport* Viewport = ViewportClient->Viewport;

	// Check if the viewport size is a valid value to prevent division by zero
	if (Viewport ? Viewport->GetSizeXY().GetMin() <= 0 : true)
	{
		return false;
	}

	const int32 ViewportInteractX = Viewport->GetMouseX();
	const int32 ViewportInteractY = Viewport->GetMouseY();
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		Viewport,
		ViewportClient->GetScene(),
		ViewportClient->EngineShowFlags).SetRealtimeUpdate(ViewportClient->IsRealtime()));
	const FSceneView* SceneView = ViewportClient->CalcSceneView(&ViewFamily);

	const FViewportCursorLocation MouseViewportRay(SceneView, ViewportClient, ViewportInteractX, ViewportInteractY);
	const FVector& MouseViewportRayOrigin = MouseViewportRay.GetOrigin();
	const FVector& MouseViewportRayDirection = MouseViewportRay.GetDirection();

	Start = MouseViewportRayOrigin;
	Direction = MouseViewportRayDirection;

	// If we're dealing with an orthographic view, push the origin of the ray backward along the viewport forward axis
	// to make sure that we can detect objects that are behind the origin!
	if (!ViewportClient->IsPerspective())
	{
		Start -= MouseViewportRayDirection * FLT_MAX;
	}

	return true;
}

// Helper function to move all actors behind the camera
void UGHEditorSubsystem::MoveActorsBehindCamera(AActor* Actor, const bool bExtendRange)
{
	TArray<TObjectPtr<AActor>> Actors;
	Actors.Add(Actor);
	MoveActorsBehindCamera(Actors, bExtendRange);
}

void UGHEditorSubsystem::MoveActorsBehindCamera(TArray<TObjectPtr<AActor>>& Actors, const bool bExtendRange)
{
	// Get the camera location
	const FLevelEditorViewportClient* ViewportClient = GCurrentLevelEditingViewportClient;
	const FVector CameraLocation = ViewportClient->GetViewLocation();

	// Calculate the forward vector of the camera and then invert it to get the backward vector
	const FVector ForwardVector = ViewportClient->GetViewRotation().Vector();
	const FVector NewLocation = CameraLocation - ForwardVector * (bExtendRange ? 1000.f : 1.f);
	

	// Move all actors behind the camera
	for (AActor* Actor : Actors)
	{
		Actor->SetActorLocation(NewLocation);
	}
}

void UGHEditorSubsystem::PlaceAssetFromContentBrowser(bool bIsRespectingCollision)
{
	// Gather selected assets from the content browser
	TArray<FAssetData> AssetDataList;
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	IContentBrowserSingleton& ContentBrowserSingleton = ContentBrowserModule.Get();
	ContentBrowserSingleton.GetSelectedAssets(AssetDataList);

	FSlateApplication& SlateApp = FSlateApplication::Get();
	FWidgetPath WidgetPath = SlateApp.LocateWindowUnderMouse(SlateApp.GetCursorPos(), SlateApp.GetInteractiveTopLevelWindows());
	
	if (IsViewportUnderCursor())
	{
		SlateApp.SetUserFocus(0, WidgetPath, EFocusCause::SetDirectly);
	}

	TMap<IAssetFactoryInterface*, TArray<FTypedElementHandle>> PlacedElementsByFactory;

	FPlacementOptions PlacementOptions;
	PlacementOptions.bIsCreatingPreviewElements = false;
	PlacementOptions.bPreferBatchPlacement = false;

	FLevelEditorViewportClient* ViewportClient = GCurrentLevelEditingViewportClient;
	
	// PlacementOptions.
	bool bAtLeastOneAssetPlaceable = false;
	for (FAssetData& AssetData : AssetDataList)
	{
		UPlacementSubsystem* PlacementSubsystem = GEditor->GetEditorSubsystem<UPlacementSubsystem>();
		TScriptInterface<IAssetFactoryInterface> FactoryInterface = PlacementSubsystem->FindAssetFactoryFromAssetData(AssetData);
		if (FactoryInterface.GetObject() != nullptr)
		{
			bAtLeastOneAssetPlaceable = true;
		}
	}

	if (!bAtLeastOneAssetPlaceable)
	{
		PlaceAssetsInViewport(bIsRespectingCollision);
		return;
	}

	// Depending on the settings, we either place the actors at the camera location or at the hit location
	FHitResult HitResult;
	const bool bDidHit = PerformViewportRaycast(GCurrentLevelEditingViewportClient, HitResult);
	if (!bDidHit && !UGHDeveloperSettings::Get()->bPlaceActorsAtCameraIfNoHit)
	{
		return;
	}

	for (FAssetData& AssetData : AssetDataList)
	{
		// Get the UPlacementSubsystem
		UPlacementSubsystem* PlacementSubsystem = GEditor->GetEditorSubsystem<UPlacementSubsystem>();
		TScriptInterface<IAssetFactoryInterface> FactoryInterface = PlacementSubsystem->FindAssetFactoryFromAssetData(AssetData);
		UActorFactory* ActorFactory = Cast<UActorFactory>(FactoryInterface.GetObject());
		
		const FViewportCursorLocation MouseViewportRay = GetViewportCursorLocation();

		// Tell the viewport to drop the object at the cursor position
		TArray ObjectsToPlace{AssetData.GetAsset()};
		TArray<AActor*> OutActors;
		ViewportClient->DropObjectsAtCoordinates(MouseViewportRay.GetCursorPos().X, MouseViewportRay.GetCursorPos().Y, ObjectsToPlace, OutActors, false, false, false /*@TODO: Make it a setting later? */, ActorFactory);

		for (AActor* SpawnedActor : OutActors)
		{
			if (SpawnedActor == nullptr)
			{
				continue;
			}

			LastSpawnedActor = SpawnedActor;
		}

		// If we don't respect collision, we're done here
		if (!bIsRespectingCollision)
		{
			return;
		}
		
		for (AActor* Actor : OutActors)
		{
			if (Actor == nullptr)
			{
				continue;
			}
			
			const FVector StartingLocation = Actor->GetActorLocation();
			MoveActorsBehindCamera(Actor);
			
			// Move the actor to the hit result location
			FHitResult SweepHitResult;
			Actor->SetActorLocation(StartingLocation, true, &SweepHitResult);

			if (SweepHitResult.bBlockingHit)
			{
				Actor->SetActorLocation(SweepHitResult.Location);
			}
		}
	}
}

void UGHEditorSubsystem::PlaceAssetsInViewport(bool bIsRespectingCollision)
{
	if (GEditor->GetSelectedActors()->Num() <= 0)
	{
#if IS_GIZMO_DEBUG
		GIZMO_LOG(Warning, TEXT("Executing Gizmo Action failed: No actors selected."));
#endif
		return;
	}
	
	FLevelEditorViewportClient* ViewportClient = GCurrentLevelEditingViewportClient;
	if (!GizmoHelperVars.IsValid())
	{
		TArray<AActor*> SelectedActors, AllActors, SelectedChildActors, ActorsToMoveCopy;
		GEditor->GetSelectedActors()->GetSelectedObjects<AActor>(SelectedActors);
		GizmoHelperVars = MakeShared<FGizmoHelperVars>(SelectedActors);
	}
	
	// Store the relative positions of the actors
	TArray<FGHActorRelativePosition> CachedRelativePositions;
	for (AActor* Actor : GizmoHelperVars->ActorsToMoveCopy)
	{
		FGHActorRelativePosition RelativePosition;
		RelativePosition.Actor = Actor;
		RelativePosition.RelativePosition = Actor->GetActorLocation() - GizmoHelperVars->FirstActor->GetActorLocation();
		CachedRelativePositions.Add(RelativePosition);
	}

	// mark these as modified
	GEditor->GetSelectedActors()->Modify();
	for (AActor* Actor : GizmoHelperVars->AllActors)
	{
		Actor->Modify();
	}
	
	// Perform a raycast to get the hit result
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore = GetActorsToIgnore(GizmoHelperVars->AllActors);
	const bool bTraceSuccess = PerformViewportRaycast(ViewportClient, HitResult, ActorsToIgnore);

	// If the trace is not successful, return
	if (!bTraceSuccess)
	{
#if IS_GIZMO_DEBUG
		GIZMO_LOG(Verbose, TEXT("Executing Gizmo Action failed: Cursor is not over a valid location."));
#endif
		return;
	}

	// Move actors behind the camera to avoid collision
	MoveActorsBehindCamera(GizmoHelperVars->AllActors);

	// Move all actors except the first one far behind to avoid overlaps
	for (AActor* Child : GizmoHelperVars->ActorsToMoveCopy)
	{
		const FVector CameraLocation = ViewportClient->GetViewLocation() + ViewportClient->GetViewRotation().Vector() * -100000.f;
		Child->SetActorLocation(CameraLocation);
	}

	// Store the target location
	FVector TargetLocation = HitResult.Location;
	LastMouseHitLocation = TargetLocation;

	// Get the first actor
	if (GizmoHelperVars->FirstActor == nullptr)
	{
#if IS_GIZMO_DEBUG
		GIZMO_LOG(Error, TEXT("Executing Gizmo Action failed: Couldn't find the first actor."));
#endif
		return;
	}

	// move the first actor to the target location
	if (bIsRespectingCollision)
	{
		// Move the first actor to the target location others to first actor location
		// this is done to easily restore actors' relative positions
		bool bUseNativeCollision = false;
		GetBoundingBox(GizmoHelperVars->FirstActor, bUseNativeCollision);

		if (bUseNativeCollision)
		{
			// Move the actor to the target location
			FHitResult SweepHitResult;
			GizmoHelperVars->FirstActor->SetActorLocation(TargetLocation, true, &SweepHitResult);

			// If the actor is blocking, move it to the blocking location
			if (SweepHitResult.bBlockingHit)
			{
				GizmoHelperVars->FirstActor->SetActorLocation(SweepHitResult.Location);
			}
		}
		else
		{
			PlaceVirtualObject(GizmoHelperVars->FirstActor, TargetLocation, HitResult);
		}
	}
	else
	{
		// Move the first actor to the target location
		GizmoHelperVars->FirstActor->SetActorLocation(TargetLocation);
	}

	// refresh pivot location
	UpdateGizmoToActorLocation(GizmoHelperVars->FirstActor);

	// Restore actors' relative positions
	for (auto& Cached : CachedRelativePositions)
	{
		const FVector NewPosition = GizmoHelperVars->FirstActor->GetActorLocation() + Cached.RelativePosition;
		Cached.Actor->SetActorLocation(NewPosition);
	}
#if IS_GIZMO_DEBUG
	GIZMO_LOG(Log, TEXT("Executing Gizmo Action completed!"));
#endif
}

EGizmoSelectionMode::Type UGHEditorSubsystem::GetSelectionMode(const TSharedPtr<SWidget>& Widget)
{
	if (
		Widget->GetType() == GH_CONTENT_BROWSER_LIST ||
		Widget->GetType() == GH_CONTENT_BROWSER_TILE ||
		Widget->GetType() == GH_CONTENT_BROWSER_COLUMN
		)
	{
		return EGizmoSelectionMode::ContentBrowser;
	}

	// Anything that is not the content browser should be treated as a viewport
	return EGizmoSelectionMode::Viewport;
}

FBox UGHEditorSubsystem::GetBoundingBox(const AActor* RelevantActor, bool& bIsCustom)
{
	// Get the bounds of the actor
	FVector Origin, Extent;
	RelevantActor->GetActorBounds(false, Origin, Extent, true);
	const FBox ActorBounds = FBox((Origin - Extent), (Origin + Extent));
	FBox BoundingBoxEd, BoundingBox;
	RelevantActor->GetStreamingBounds(BoundingBox, BoundingBoxEd);

	// Decide whether to use native or virtual collision
	bool bUseNativeCollision = true;
	for (const FGHActorCustomBounds& CustomBound : UGHDeveloperSettings::Get()->CustomBounds)
	{
		if (!RelevantActor->GetClass()->IsChildOf(CustomBound.ActorClass))
		{
			continue;
		}
		
		
		bUseNativeCollision = false;
		break;
	}

	
	if (bUseNativeCollision)
	{
		for (const auto& ActorC : UGHDeveloperSettings::Get()->InternalVirtualActors)
		{
			if (!RelevantActor->GetClass()->IsChildOf(ActorC))
			{
				continue;
			}
			
			bUseNativeCollision = false;
			break;
		}

		if (Cast<UBlueprintGeneratedClass>(RelevantActor->GetClass()))
		{
			bUseNativeCollision = false;
		}
	}
	
	bIsCustom = bUseNativeCollision;

	
	if (BoundingBox.GetExtent() / 2 == ActorBounds.GetExtent())
	{
		return ActorBounds;
	}
	return BoundingBox;
}


void UGHEditorSubsystem::PlaceVirtualObject(AActor* Actor, const FVector& TargetLocation, const FHitResult& HitResult) const
{
	/* Calculate the target location */
	FVector ComputedTargetLocation = TargetLocation;
	FVector Offset = FVector::ZeroVector;
	
	/* Check if there is any custom bounds scale for the actor class */
	TArray<TSubclassOf<AActor>> Keys;
	bool bFoundCustomBounds = false;
	
	for (const FGHActorCustomBounds& CustomBound : UGHDeveloperSettings::Get()->CustomBounds)
	{
		if (!Actor->GetClass()->IsChildOf(CustomBound.ActorClass))
		{
			continue;
		}
		
		const FBoxSphereBounds FoundBounds = CustomBound.CustomBounds;
		Offset = FVector(
		HitResult.ImpactNormal.X * FoundBounds.BoxExtent.X,
		HitResult.ImpactNormal.Y * FoundBounds.BoxExtent.Y,
		HitResult.ImpactNormal.Z * FoundBounds.BoxExtent.Z);
		bFoundCustomBounds = true;
		break;
	}
	
	FVector RelativeBoundsOffset = FVector::ZeroVector;
	
	/** If no custom bounds scale was found, use the native bounds */
	if (!bFoundCustomBounds)
	{
		RelativeBoundsOffset = GizmoHelperVars->BoundingBox.Offset;
		Offset = FVector(
		HitResult.ImpactNormal.X * GizmoHelperVars->BoundingBox.Extent.X,
		HitResult.ImpactNormal.Y * GizmoHelperVars->BoundingBox.Extent.Y,
		HitResult.ImpactNormal.Z * GizmoHelperVars->BoundingBox.Extent.Z);
	}

	// Move the actor to the computed target location
	ComputedTargetLocation = ComputedTargetLocation + Offset + RelativeBoundsOffset;
	
	Actor->SetActorLocation(ComputedTargetLocation);
}

bool UGHEditorSubsystem::PerformViewportRaycast(const FLevelEditorViewportClient* EditorViewportClient, FHitResult& HitResult, const TArray<AActor*>& ActorsToIgnore)
{
	// Traces to mouse position
	FVector TraceStart, Direction;
	GetMouseHitLocation(TraceStart, Direction);

	// Trace end is a point far away from the trace start
	const FVector TraceEnd = TraceStart + (Direction * UE_BIG_NUMBER);
	
	FCollisionQueryParams Params;
	Params.bTraceComplex = UGHDeveloperSettings::Get()->TraceToComplexCollision();
	Params.AddIgnoredActors(ActorsToIgnore);

	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.Visibility = ECR_Block;
	ResponseParams.CollisionResponse.WorldDynamic = ECR_Block;
	ResponseParams.CollisionResponse.WorldStatic = ECR_Block;

	constexpr ECollisionChannel TraceChannelProperty = ECC_Visibility;
	return EditorViewportClient->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, TraceChannelProperty, Params, ResponseParams);
}

TArray<AActor*> UGHEditorSubsystem::GetActorsToIgnore(TArray<TObjectPtr<AActor>>& Actors)
{
	// Get all actors to ignore
	TArray<AActor*> ActorsToIgnore;
	for (AActor* Actor : Actors)
	{
		// Get root component's children
		TArray<USceneComponent*> ChildrenComponents;
		Actor->GetRootComponent()->GetChildrenComponents(true, ChildrenComponents);
		ActorsToIgnore.Add(Actor);

		// Find child actors within the Actors array
		for (USceneComponent* ChildComponent : ChildrenComponents)
		{
			AActor* ChildActor = ChildComponent->GetOwner();
			if (ChildActor == Actor)
			{
				continue;
			}

			ActorsToIgnore.Add(ChildActor);
		}
	}

	return ActorsToIgnore;
}


#undef LOCTEXT_NAMESPACE
