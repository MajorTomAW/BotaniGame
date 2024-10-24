// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BotaniIndicatorDescriptor.generated.h"

class UBotaniIndicatorDescriptor;
class UBotaniIndicatorManagerComponent;

struct FIndicatorProjection
{
	bool Project(const UBotaniIndicatorDescriptor& IndicatorDescriptor, const FSceneViewProjectionData& InProjectionData, const FVector2f& ScreenSize, FVector& ScreenPositionWithDepth);
};

UENUM(BlueprintType)
enum class EActorCanvasProjectionMode : uint8
{
	ComponentPoint,
	ComponentBoundingBox,
	ComponentScreenBoundingBox,
	ActorBoundingBox,
	ActorScreenBoundingBox
};

/**
 * UBotaniIndicatorDescriptor
 *
 * Describes and controls an active indicator.
 * It is highly recommended that our widget implements IActorIndicatorWidget so that it can 'bind' to the associated data.
 */
UCLASS(BlueprintType)
class BOTANIUI_API UBotaniIndicatorDescriptor : public UObject
{
	GENERATED_BODY()

public:
	UBotaniIndicatorDescriptor();

public:
	/** Data object associated with this indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	UObject* GetDataObject() const { return DataObject; }

	/** Set the data object associated with this indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void SetDataObject(UObject* InDataObject) { DataObject = InDataObject; }


	/** Scene component associated with this indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	USceneComponent* GetSceneComponent() const { return Component; }

	/** Set the scene component associated with this indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void SetSceneComponent(USceneComponent* InComponent) { Component = InComponent; }


	/** Component socket name associated with this indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	FName GetComponentSocketName() const { return ComponentSocketName; }

	/** Set the component socket name associated with this indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void SetComponentSocketName(FName SocketName) { ComponentSocketName = SocketName; }


	/** Indicator widget associated with this indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	TSoftClassPtr<UUserWidget> GetIndicatorClass() const { return IndicatorWidgetClass; }

	/** Set the indicator widget associated with this indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void SetIndicatorClass(TSoftClassPtr<UUserWidget> InIndicatorWidgetClass)
	{
		IndicatorWidgetClass = InIndicatorWidgetClass;
	}

public:
	TWeakObjectPtr<UUserWidget> IndicatorWidget;

public:
	/** Set whether the indicator should be automatically removed when the indicator component is null. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void SetAutoRemoveWhenIndicatorComponentIsNull(bool CanAutomaticallyRemove)
	{
		bAutoRemoveWhenIndicatorComponentIsNull = CanAutomaticallyRemove;
	}

	/** Get whether the indicator should be automatically removed when the indicator component is null. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	bool GetAutoRemoveWhenIndicatorComponentIsNull() const { return bAutoRemoveWhenIndicatorComponentIsNull; }

	/** Get whether the indicator can be automatically removed. */
	bool CanAutomaticallyRemove() const
	{
		return bAutoRemoveWhenIndicatorComponentIsNull && !IsValid(GetSceneComponent());
	}

public:
	/** Get whether the indicator is visible. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	bool GetIsVisible() const { return IsValid(GetSceneComponent()) && bVisible; }

	/** Set the desired visibility of the indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void SetDesiredVisibility(bool InVisible)
	{
		bVisible = InVisible;
	}

	
	/** Get the projection mode of the indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	EActorCanvasProjectionMode GetProjectionMode() const { return ProjectionMode; }

	/** Set the projection mode of the indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void SetProjectionMode(EActorCanvasProjectionMode InProjectionMode)
	{
		ProjectionMode = InProjectionMode;
	}


	/** Get the horizontal alignment of the indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	EHorizontalAlignment GetHorizontalAlignment() const { return HAlignment; }

	/** Set the horizontal alignment of the indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment)
	{
		HAlignment = InHorizontalAlignment;
	}

	
	/** Get the vertical alignment of the indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	EVerticalAlignment GetVerticalAlignment() const { return VAlignment; }

	/** Set the vertical alignment of the indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void SetVerticalAlignment(EVerticalAlignment InVerticalAlignment)
	{
		VAlignment = InVerticalAlignment;
	}

	
	/** Get whether the indicator should be clamped to the edge of the screen. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	bool GetClampToScreen() const { return bClampToScreen; }

	/** Set whether the indicator should be clamped to the edge of the screen. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void SetClampToScreen(bool bValue)
	{
		bClampToScreen = bValue;
	}


	/** Get whether to show an arrow if clamping to the edge of the screen. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	bool GetShowClampToScreenArrow() const { return bShowClampToScreenArrow; }

	/** Set whether to show an arrow if clamping to the edge of the screen. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void SetShowClampToScreenArrow(bool bValue)
	{
		bShowClampToScreenArrow = bValue;
	}


	/** Get the world position offset for the indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	FVector GetWorldPositionOffset() const { return WorldPositionOffset; }

	/** Set the world position offset for the indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void SetWorldPositionOffset(FVector Offset)
	{
		WorldPositionOffset = Offset;
	}


	/** Get the position offset for the indicator in screen space. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	FVector2D GetScreenSpaceOffset() const { return ScreenSpaceOffset; }

	/** Set the position offset for the indicator in screen space. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void SetScreenSpaceOffset(FVector2D Offset)
	{
		ScreenSpaceOffset = Offset;
	}


	/** Get the bounding box anchor for the indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	FVector GetBoundingBoxAnchor() const { return BoundingBoxAnchor; }

	/** Set the bounding box anchor for the indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void SetBoundingBoxAnchor(FVector InBoundingBoxAnchor)
	{
		BoundingBoxAnchor = InBoundingBoxAnchor;
	}


	/**
	 * Get the priority of the indicator.
	 * Can be used to sort indicators to allow some group of indicators to be drawn on top of others.
	 */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	int32 GetPriority() const { return Priority; }

	/** Set the priority of the indicator. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void SetPriority(int32 InPriority)
	{
		Priority = InPriority;
	}

public:
	/** Gets the indicator manager component associated with this indicator. */
	UBotaniIndicatorManagerComponent* GetIndicatorManagerComponent() const {return ManagerPtr.Get(); }

	/** Sets the indicator manager component associated with this indicator. */
	void SetIndicatorManagerComponent(UBotaniIndicatorManagerComponent* InManagerPtr);


	/** Unregisters this indicator from the indicator manager. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void UnregisterIndicator();

private:
	UPROPERTY()
	bool bVisible = true;
	UPROPERTY()
	bool bClampToScreen = false;
	UPROPERTY()
	bool bShowClampToScreenArrow = false;
	UPROPERTY()
	bool bOverrideScreenPosition = false;
	UPROPERTY()
	bool bAutoRemoveWhenIndicatorComponentIsNull = false;

	UPROPERTY()
	EActorCanvasProjectionMode ProjectionMode = EActorCanvasProjectionMode::ComponentPoint;
	UPROPERTY()
	TEnumAsByte<EHorizontalAlignment> HAlignment = HAlign_Center;
	UPROPERTY()
	TEnumAsByte<EVerticalAlignment> VAlignment = VAlign_Center;

	UPROPERTY()
	int32 Priority = 0;

	UPROPERTY()
	FVector BoundingBoxAnchor = FVector(0.5, 0.5, 0.5);
	UPROPERTY()
	FVector2D ScreenSpaceOffset = FVector2D(0, 0);
	UPROPERTY()
	FVector WorldPositionOffset = FVector(0, 0, 0);

private:
	friend class SActorCanvas;

	UPROPERTY()
	TObjectPtr<UObject> DataObject;
	
	UPROPERTY()
	TObjectPtr<USceneComponent> Component;

	UPROPERTY()
	FName ComponentSocketName = NAME_None;

	UPROPERTY()
	TSoftClassPtr<UUserWidget> IndicatorWidgetClass;

	UPROPERTY()
	TWeakObjectPtr<UBotaniIndicatorManagerComponent> ManagerPtr;

	TWeakPtr<SWidget> Content;
	TWeakPtr<SWidget> CanvasHost;
};
