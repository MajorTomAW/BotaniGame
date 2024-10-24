// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Slate/SCircumferenceMarkerWidget.h"
#include "CircumferenceMarkerWidget.generated.h"



UCLASS(MinimalAPI)
class UCircumferenceMarkerWidget : public UWidget
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UWidget Interface
	BOTANIUI_API virtual void SynchronizeProperties() override;
protected:
	BOTANIUI_API virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

public:
	//~ Begin UVisual Interface
	BOTANIUI_API virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

public:
	/** The list of positions/orientations to draw the markers at. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Appearance")
	TArray<FCircumferenceMarkerEntry> MarkerList;

	/** The radius of the circle. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance, meta=(ClampMin=0.0))
	float Radius = 48.0f;

	/** The marker image to place around the circle. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FSlateBrush MarkerImage;

	/** Whether reticle corner images are placed outside the spread radius */
	//@TODO: Make this a 0-1 float alignment instead (e.g., inside/on/outside the radius)?
	UPROPERTY(EditAnywhere, Category=Corner)
	uint8 bReticleCornerOutsideSpreadRadius : 1;

public:
	/** Sets the radius of the circle. */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
	void SetRadius(float InRadius);
	
private:
	/** Internal slate widget representing the actual marker visuals */
	TSharedPtr<SCircumferenceMarkerWidget> MyMarkerWidget;
};
