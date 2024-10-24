// Copyright © 2024 MajorT. All rights reserved.

#include "PlaysetRootActor.h"

#include "Widgets/Layout/SPopup.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(PlaysetRootActor)

APlaysetRootActor::APlaysetRootActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SceneRootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneRootComponent"));
	RootComponent = SceneRootComponent;
}

#if WITH_EDITORONLY_DATA
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void APlaysetRootActor::UpdateAssociatedPlayset()
{
	// Display a popup to ask the user if they want to update the associated playset
	TSharedRef<SPopup> Popup = SNew(SPopup)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Do you want to update the associated playset?")))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(10)
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Yes")))
					// .OnClicked(this, &APlaysetRootActor::OnUpdateAssociatedPlaysetYes)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(10)
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("No")))
					// .OnClicked(this, &APlaysetRootActor::OnUpdateAssociatedPlaysetNo)
				]
			]
		];

	// show the popup
	// FSlateApplication::Get().GetPopupSupport()->
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#endif

void APlaysetRootActor::Destroyed()
{
#if WITH_EDITOR
	if (GEngine->IsEditor())
	{
		TArray<AActor*> AttachedActors;
		GetAttachedActors(AttachedActors);

		for (AActor* AttachedActor : AttachedActors)
		{
			AttachedActor->Destroy();
		}
	}
#endif

	Super::Destroyed();
}
