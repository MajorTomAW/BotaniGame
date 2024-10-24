// Copyright © 2024 MajorT. All rights reserved.

#include "ActorFactory_Playset.h"

#include "EngineUtils.h"
#include "GamePlaysetsEditor.h"
#include "Playset.h"
#include "PlaysetRootActor.h"
#include "Elements/Actor/ActorElementData.h"
#include "Subsystems/PlacementSubsystem.h"

UActorFactory_Playset::UActorFactory_Playset(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DisplayName = NSLOCTEXT("GamePlaysets", "ActorFactory_Playset", "Playset Root");
	NewActorClass = APlaysetRootActor::StaticClass();

	bShowInEditorQuickMenu = true;
	bUseSurfaceOrientation = true;
	bUsePlacementExtent = true;
}

AActor* UActorFactory_Playset::GetDefaultActor(const FAssetData& AssetData)
{
	UPlayset* Playset = Cast<UPlayset>(AssetData.GetAsset());
	if (Playset == nullptr)
	{
		return nullptr;
	}

	if (Playset->ActorClassCount.Num() <= 1)
	{
		for (const auto& KVP : Playset->ActorClassCount)
		{
			if (KVP.Value == 1)
			{
				UClass* ActorClass = KVP.Key.LoadSynchronous();
				return ActorClass->GetDefaultObject<AActor>();
			}
		}
	}

	
	//@TODO: Maybe in the future we want to have custom APlaysetRootActor classes per UPlayset type
	return APlaysetRootActor::StaticClass()->GetDefaultObject<APlaysetRootActor>();
}

bool UActorFactory_Playset::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
	if (!AssetData.IsValid() ||!AssetData.IsInstanceOf(UPlayset::StaticClass()))
	{
		OutErrorMsg = NSLOCTEXT("GamePlaysets", "CanCreateActorFrom_NoPlayset", "No Playset was specified.");
		return false;
	}

	const UPlayset* Playset = Cast<UPlayset>(AssetData.GetAsset());
	if (!Playset)
	{
		OutErrorMsg = NSLOCTEXT("GamePlaysets", "CanCreateActorFrom_NoPlayset", "No Playset was specified.");
		return false;
	}

	if (Playset->bHasCustomActorFactory)
	{
		return false;
	}

	return true;
}

void UActorFactory_Playset::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	Super::PostSpawnActor(Asset, NewActor);

	UPlayset* Playset = Cast<UPlayset>(Asset);
	if (!ensure(Playset))
	{
		return;
	}

	FString ActorLabel = Playset->DisplayInfo.ItemName.ToString();

	// Find the number of actors that are already spawned in the level
	UWorld* World = NewActor->GetWorld();
	int32 NumActors = 0;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		if (It->GetClass() == NewActor->GetClass())
		{
			NumActors++;
		}
	}

	// Append the number of actors to the label
	ActorLabel.AppendInt(NumActors);
	
	NewActor->SetActorLabel(ActorLabel);
	NewActor->SetActorRotation(Playset->DefaultRotation);

	if (Playset->bUseLocationOffset)
	{
		NewActor->SetActorLocation(NewActor->GetActorLocation() + Playset->LocationOffset);
	}

	if (APlaysetRootActor* PlaysetActor = Cast<APlaysetRootActor>(NewActor))
	{
		PlaysetActor->Playset = Playset;
	}
}

void UActorFactory_Playset::EndPlacement(TArrayView<const FTypedElementHandle> InPlacedElements,const FPlacementOptions& InPlacementOptions)
{
	Super::EndPlacement(InPlacedElements, InPlacementOptions);

	if (InPlacementOptions.bIsCreatingPreviewElements)
	{
		HandlePreviewPlacement_Internal(InPlacedElements, InPlacementOptions);
	}
	else
	{
		HandlePlacement_Internal(InPlacedElements, InPlacementOptions);
	}
}

void UActorFactory_Playset::HandlePlacement_Internal(TArrayView<const FTypedElementHandle> InPlacedElements, const FPlacementOptions& InPlacementOptions)
{
	AActor* PlacedActor = GetActorFromHandle(InPlacedElements);
	if (!PlacedActor->IsA(APlaysetRootActor::StaticClass()))
	{
		return;
	}
	

	// Remove any attached actors that might have been added during preview placement
	TArray<AActor*> AttachedActors;
	PlacedActor->GetAttachedActors(AttachedActors);
	AttachedActors.Append(SpawnedPreviewActors);

	for (AActor* AttachedActor : AttachedActors)
	{
		AttachedActor->Destroy();
	}

	// Get the root actor and playset
	APlaysetRootActor* PlaysetActor = Cast<APlaysetRootActor>(PlacedActor);
	const UPlayset* Playset = PlaysetActor ? PlaysetActor->Playset : nullptr;

	// Spawn the playset actors
	SpawnPlaysetActors(PlaysetActor, Playset, PlaysetActor->GetLevel());
}

void UActorFactory_Playset::HandlePreviewPlacement_Internal(TArrayView<const FTypedElementHandle> InPlacedElements, const FPlacementOptions& InPlacementOptions)
{
	AActor* PlacedActor = GetActorFromHandle(InPlacedElements);
	if (!PlacedActor->IsA(APlaysetRootActor::StaticClass()))
	{
		return;
	}

	// Remove any attached actors that might have been added during preview placement
	TArray<AActor*> AttachedActors;
	PlacedActor->GetAttachedActors(AttachedActors);

	for (AActor* AttachedActor : AttachedActors)
	{
		AttachedActor->Destroy();
	}

	// Get the root actor and playset
	APlaysetRootActor* PlaysetActor = Cast<APlaysetRootActor>(PlacedActor);
	if (PlaysetActor == nullptr)
	{
		return;
	}
	
	const UPlayset* Playset = PlaysetActor ? PlaysetActor->Playset : nullptr;

	// Spawn the playset actors
	SpawnPlaysetPreviewActors(PlaysetActor, Playset, PlaysetActor->GetLevel());
}

void UActorFactory_Playset::PostPlaceAsset(TArrayView<const FTypedElementHandle> InHandle, const FAssetPlacementInfo& InPlacementInfo, const FPlacementOptions& InPlacementOptions)
{
	Super::PostPlaceAsset(InHandle, InPlacementInfo, InPlacementOptions);
}

UObject* UActorFactory_Playset::GetAssetFromActorInstance(AActor* ActorInstance)
{
	check(ActorInstance->IsA(NewActorClass));
	const APlaysetRootActor* PlaysetActor = Cast<APlaysetRootActor>(ActorInstance);

	if (PlaysetActor->Playset)
	{
		return PlaysetActor->Playset;
	}

	return nullptr;
}

void UActorFactory_Playset::SpawnPlaysetActors(AActor* RootActor, const UPlayset* Playset, const ULevel* InLevel)
{
	// No root actor for single-actored playsets
	if (RootActor == nullptr)
	{
		return;
	}
	
	check(Playset);
	check(InLevel);
	
	for (const auto& KVP : Playset->ActorClassCount)
	{
		for (int i = 0; i < KVP.Value; i++)
		{
			bool bAllowTemplate = false;
			const FSoftObjectPath& ActorClassPath = KVP.Key.ToSoftObjectPath();
			
			auto Predicate = [i, ActorClassPath, bAllowTemplate](const FPlaysetActorData& Data)
			{
				if (Data.ActorClass.ToSoftObjectPath() != ActorClassPath )
				{
					return false;
				}

				return bAllowTemplate ?
					(Data.ActorTemplateID == i || Data.ActorTemplateID == -1) :
					(Data.ActorTemplateID == i);
			};
			
			
			const FPlaysetActorData* ActorData = Playset->ActorData.FindByPredicate(Predicate);

			if (ActorData == nullptr)
			{
				// Try finding an actor data with template id of -1
				bAllowTemplate = true;
				ActorData = Playset->ActorData.FindByPredicate(Predicate);
			}
			
			if (ActorData == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to find actor data for class %s with template id %d"), *ActorClassPath.ToString(), i);
				continue;
			}
			
			UClass* ActorClass = KVP.Key.LoadSynchronous();
			AActor* NewActor = InLevel->GetWorld()->SpawnActor<AActor>(ActorClass, RootActor->GetActorLocation(), RootActor->GetActorRotation());

			// Apply the actor data to the new actor
			NewActor->SetActorRelativeLocation(ActorData->RelativeLocation);
			NewActor->SetActorRelativeRotation(ActorData->RelativeRotation);
			NewActor->SetActorRelativeScale3D(ActorData->RelativeScale);
			NewActor->GetRootComponent()->Mobility = EComponentMobility::Movable;
			
			NewActor->AttachToActor(RootActor, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

void UActorFactory_Playset::SpawnPlaysetPreviewActors(AActor* RootActor, const UPlayset* Playset, const ULevel* InLevel)
{
	// No root actor for single-actored playsets
	if (RootActor == nullptr)
	{
		return;
	}
	
	check(Playset);
	check(InLevel);
	
	for (const auto& KVP : Playset->ActorClassCount)
	{
		for (int i = 0; i < KVP.Value; i++)
		{
			bool bAllowTemplate = false;
			const FSoftObjectPath& ActorClassPath = KVP.Key.ToSoftObjectPath();
			
			auto Predicate = [i, ActorClassPath, bAllowTemplate](const FPlaysetActorData& Data)
			{
				if (Data.ActorClass.ToSoftObjectPath() != ActorClassPath )
				{
					return false;
				}

				return bAllowTemplate ?
					(Data.ActorTemplateID == i || Data.ActorTemplateID == -1) :
					(Data.ActorTemplateID == i);
			};
			
			
			const FPlaysetActorData* ActorData = Playset->ActorData.FindByPredicate(Predicate);

			if (ActorData == nullptr)
			{
				// Try finding an actor data with template id of -1
				bAllowTemplate = true;
				ActorData = Playset->ActorData.FindByPredicate(Predicate);
			}
			
			if (ActorData == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to find actor data for class %s with template id %d"), *ActorClassPath.ToString(), i);
				continue;
			}
			
			UClass* ActorClass = KVP.Key.LoadSynchronous();
			AActor* NewActor = InLevel->GetWorld()->SpawnActor<AActor>(ActorClass, RootActor->GetActorLocation(), RootActor->GetActorRotation());
			NewActor->SetActorEnableCollision(false);

			// Apply the actor data to the new actor
			NewActor->SetActorRelativeLocation(ActorData->RelativeLocation);
			NewActor->SetActorRelativeRotation(ActorData->RelativeRotation);
			NewActor->SetActorRelativeScale3D(ActorData->RelativeScale);
			NewActor->GetRootComponent()->Mobility = EComponentMobility::Movable;
			
			NewActor->AttachToActor(RootActor, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

AActor* UActorFactory_Playset::GetActorFromHandle(TArrayView<const FTypedElementHandle> InHandle)
{
	AActor* PlacedActor = nullptr;

	// Find the placed actor
	for (const auto& Handle : InHandle)
	{
		if (!Handle.IsSet())
		{
			continue;
		}

		PlacedActor = ActorElementDataUtil::GetActorFromHandle(Handle);
		if (ensure(PlacedActor))
		{
			break;
		}
	}

	return PlacedActor;
}

bool UActorFactory_Playset::HasPlaysetJustOneActor(const UPlayset* Playset) const
{
	bool bResult = false;

	if (Playset->ActorClassCount.Num() <= 1)
	{
		for (const auto& KVP : Playset->ActorClassCount)
		{
			if (KVP.Value == 1)
			{
				bResult = true;
			}
			else
			{
				bResult = false;
				break;
			}
		}
	}

	return bResult;
}
