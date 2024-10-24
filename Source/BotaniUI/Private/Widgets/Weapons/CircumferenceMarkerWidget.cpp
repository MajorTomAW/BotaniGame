// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Widgets/Weapons/CircumferenceMarkerWidget.h"

UCircumferenceMarkerWidget::UCircumferenceMarkerWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UWidget::SetVisibility(ESlateVisibility::HitTestInvisible);
	bIsVolatile = true;
}

void UCircumferenceMarkerWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyMarkerWidget->SetRadius(Radius);
	MyMarkerWidget->SetMarkerList(MarkerList);
}

TSharedRef<SWidget> UCircumferenceMarkerWidget::RebuildWidget()
{
	MyMarkerWidget = SNew(SCircumferenceMarkerWidget)
		.MarkerBrush(&MarkerImage)
		.Radius(this->Radius)
		.MarkerList(this->MarkerList);

	return MyMarkerWidget.ToSharedRef();
}

void UCircumferenceMarkerWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyMarkerWidget.Reset();
}

void UCircumferenceMarkerWidget::SetRadius(float InRadius)
{
	Radius = InRadius;
	if (MyMarkerWidget.IsValid())
	{
		MyMarkerWidget->SetRadius(InRadius);
	}
}
