// Copyright © 2024 Game Shifters Studio. All rights reserved.

#include "GizmoHelperTool/Public/GHDataDefinitions.h"

#include "GHDeveloperSettings.h"


void FGizmoBoundingBox::ConstructBoundingBox(const AActor* InActor)
{
	if (InActor == nullptr)
	{
		return;
	}

	bool bFoundCustomBounds = false;
	for (const FGHActorCustomBounds& CustomBound : UGHDeveloperSettings::Get()->CustomBounds)
	{
		if (!InActor->GetClass()->IsChildOf(CustomBound.ActorClass))
		{
			continue;
		}

		bFoundCustomBounds = true;
		Extent = CustomBound.CustomBounds.GetBox().GetExtent();
		// Origin = CustomBound.CustomBounds.GetBox().GetCenter();
		// Offset = InActor->GetActorLocation() - Origin;
	}

	if (bFoundCustomBounds)
	{
		return;
	}

	InActor->GetActorBounds(false, Origin, Extent, true);
	FBox StreamingBounds, StreamingBoundsED;
	InActor->GetStreamingBounds(StreamingBounds, StreamingBoundsED);
	Offset = InActor->GetActorLocation() - StreamingBoundsED.GetCenter();
	Extent = StreamingBoundsED.GetExtent();
}

void FGizmoHelperVars::OnConstruct(const TArray<AActor*>& InSelectedActors)
{
	SelectedActors = InSelectedActors;
	AllActors = SelectedActors;

	// Get attached child actors
	for (const AActor* Actor : SelectedActors)
	{
		Actor->GetAttachedActors(ChildActors, false, true);
	}
	
	for (AActor* Actor : ChildActors)
	{
		SelectedActors.Remove(Actor);
		AllActors.AddUnique(Actor);
	}

	if (true)
	{
		int32 i = 5;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("This is a warning message!"));
	}

	ChildActors.Empty();
	for (const AActor* Actor : SelectedActors)
	{
		Actor->GetAttachedActors(ChildActors, false, true);
	}

	// Add all actors to the list
	ActorsToMoveCopy = ChildActors;
	
	Algo::Reverse(SelectedActors);
	for (AActor* Actor : SelectedActors)
	{
		if (Actor == SelectedActors[0])
		{
			continue;
		}
		
		ActorsToMoveCopy.Insert(Actor, 0);
	}
	
	FirstActor = SelectedActors.Num() > 0 ? SelectedActors[0] : nullptr;

	BoundingBox = FGizmoBoundingBox(FirstActor);
}
