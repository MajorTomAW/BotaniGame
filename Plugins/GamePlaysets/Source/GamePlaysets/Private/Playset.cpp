// Copyright © 2024 MajorT. All rights reserved.

#include "Playset.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#include "UObject/ObjectSaveContext.h"
#include "Subsystems/EditorAssetSubsystem.h"
#endif

#include "PlaysetRootActor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Playset)

#define LOCTEXT_NAMESPACE "GamePlaysets"

UPlayset::UPlayset(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Placement
	OffsetType = EPlaysetOffsetType::Center;
	bAdjustForWorldCollision = false;
	bUseLocationOffset = false;
	SizeX = 0;
	SizeY = 0;
	SizeZ = 0;

#if WITH_EDITORONLY_DATA
	DefaultSubCategoryID = "Playset";
#endif
}

UPlayset::UPlayset(const FPlaysetDisplayInfo& InDisplayInfo, const FPlaysetDataList& InDataList)
{
	// Placement
	OffsetType = EPlaysetOffsetType::Center;
	bAdjustForWorldCollision = false;
	bUseLocationOffset = false;
	SizeX = 0;
	SizeY = 0;
	SizeZ = 0;
	
	DisplayInfo = InDisplayInfo;
	DataList = InDataList;
}

FText UPlayset::GetPlaysetDesc() const
{
	FText MultiLineDesc = DisplayInfo.ItemDescription;

	// Adjust the description text to add a new line after each "\n"
	FString DescString = MultiLineDesc.ToString();
	DescString = DescString.Replace(TEXT("\\n"), TEXT("\n"));
	MultiLineDesc = FText::FromString(DescString);
	
	return MultiLineDesc;
}

FVector UPlayset::GetPlaysetLocationOffset() const
{
	if (bUseLocationOffset)
	{
		return LocationOffset;
	}
	return FVector::ZeroVector;
}

#if WITH_EDITOR
void UPlayset::InitializeDisplayInfo(const FPlaysetDisplayInfo& InDisplayInfo)
{
	DisplayInfo = InDisplayInfo;
}

void UPlayset::InitializeDataList(const FPlaysetDataList& InDataList)
{
	DataList = InDataList;
}

void UPlayset::InitializeOffsetType(EPlaysetOffsetType InOffsetType)
{
	OffsetType = InOffsetType;
}

void UPlayset::InitializeSavedActors(TArray<FAssetData> InActorData)
{
	UWorld* World = InActorData.Top().GetAsset()->GetWorld();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AActor* PlaysetRoot = World->SpawnActor<APlaysetRootActor>(APlaysetRootActor::StaticClass(), FTransform::Identity, SpawnParams);
	
	AActor* FirstActor = Cast<AActor>(InActorData.Top().GetAsset());
	if (!ensure(FirstActor))
	{
		return;
	}

	PlaysetRoot->SetActorLocation(FirstActor->GetActorLocation());
	PlaysetRoot->SetActorRotation(FirstActor->GetActorRotation());

	FBoxSphereBounds SharedBounds;
	SharedBounds.Origin = PlaysetRoot->GetActorLocation();

	// Attach all assets to the root actor
	for (const FAssetData& FoundData : InActorData)
	{
		AActor* Actor = Cast<AActor>(FoundData.GetAsset());
		if (!ensure(Actor))
		{
			continue;
		}

		Actor->GetRootComponent()->Mobility = EComponentMobility::Movable;
		Actor->AttachToActor(PlaysetRoot, FAttachmentTransformRules::KeepWorldTransform);

		SharedBounds = SharedBounds + Actor->GetStreamingBounds();
	}

	DrawDebugBox(World, SharedBounds.Origin, SharedBounds.BoxExtent, FColor::Red, true, 1.0f, 0, 1.0f);
	// UE_LOG(LogTemp, Warning, TEXT("SharedBounds: %s"), *SharedBounds.ToString());

	// Move the playset root to the origin
	FVector PreviousLocation = PlaysetRoot->GetActorLocation();
	FVector NewLocation, CachedOffset;

	typedef TMap<EPlaysetOffsetType, FVector> FOffsetMap;
	FOffsetMap OffsetMap;
	OffsetMap.Add(EPlaysetOffsetType::Center, FVector(0.f, 0.f, 0.f));
	OffsetMap.Add(EPlaysetOffsetType::FrontLet, FVector(-1.f, 1.f, 0.f));
	OffsetMap.Add(EPlaysetOffsetType::FrontRight, FVector(-1.f, -1.f, 0.f));
	OffsetMap.Add(EPlaysetOffsetType::BackLeft, FVector(1.f, 1.f, 0.f));
	OffsetMap.Add(EPlaysetOffsetType::BackRight, FVector(1.f, -1.f, 0.f));

	{
		FVector Offset = OffsetMap[OffsetType];
		NewLocation = SharedBounds.Origin - FVector(SharedBounds.BoxExtent.X * Offset.X, SharedBounds.BoxExtent.Y * Offset.Y, SharedBounds.BoxExtent.Z);
		PreviousLocation += FVector(SharedBounds.BoxExtent.X * Offset.X, SharedBounds.BoxExtent.Y * Offset.Y, 0.f);
		CachedOffset = FVector(SharedBounds.BoxExtent.X * -Offset.X, SharedBounds.BoxExtent.Y * -Offset.Y, 0.f);
	}
	
	/*switch (OffsetType) {
	case EPlaysetOffsetType::Center:
		{
			NewLocation = SharedBounds.Origin - FVector(0, 0, SharedBounds.BoxExtent.Z);
		}
		break;
	case EPlaysetOffsetType::FrontLet:
		{
			NewLocation = SharedBounds.Origin - FVector(SharedBounds.BoxExtent.X, SharedBounds.BoxExtent.Y, SharedBounds.BoxExtent.Z);
			PreviousLocation += FVector(SharedBounds.BoxExtent.X, -SharedBounds.BoxExtent.Y, 0.f);
			CachedOffset = FVector(-SharedBounds.BoxExtent.X, SharedBounds.BoxExtent.Y, 0.f);
		}
		break;
	case EPlaysetOffsetType::FrontRight:
		{
			NewLocation = SharedBounds.Origin - FVector(-SharedBounds.BoxExtent.X, SharedBounds.BoxExtent.Y, SharedBounds.BoxExtent.Z);
			PreviousLocation += FVector(SharedBounds.BoxExtent.X, SharedBounds.BoxExtent.Y, 0.f);
			CachedOffset = FVector(SharedBounds.BoxExtent.X, SharedBounds.BoxExtent.Y, 0.f);
		}
		break;
	case EPlaysetOffsetType::BackLeft:
		{
			NewLocation = SharedBounds.Origin - FVector(SharedBounds.BoxExtent.X, -SharedBounds.BoxExtent.Y, SharedBounds.BoxExtent.Z);
			PreviousLocation += FVector(-SharedBounds.BoxExtent.X, -SharedBounds.BoxExtent.Y, 0.f);
			CachedOffset = FVector(-SharedBounds.BoxExtent.X, -SharedBounds.BoxExtent.Y, 0.f);
		}
		break;
	case EPlaysetOffsetType::BackRight:
		{
			NewLocation = SharedBounds.Origin - FVector(-SharedBounds.BoxExtent.X, -SharedBounds.BoxExtent.Y, SharedBounds.BoxExtent.Z);
			PreviousLocation += FVector(-SharedBounds.BoxExtent.X, -SharedBounds.BoxExtent.Y, 0.f);
			CachedOffset = FVector(SharedBounds.BoxExtent.X, -SharedBounds.BoxExtent.Y, 0.f);
		}
		break;
	}*/

	// PreviousLocation *= FVector(1.f, 1.f, 1.f);
	PlaysetRoot->SetActorLocation(NewLocation);
	
	// Restore the relative locations of the actors
	for (const FAssetData& FoundData : InActorData)
	{
		AActor* Actor = Cast<AActor>(FoundData.GetAsset());
		if (!ensure(Actor))
		{
			continue;
		}

		Actor->SetActorLocation(
			(Actor->GetActorLocation() + (PreviousLocation - PlaysetRoot->GetActorLocation())) +
			CachedOffset
			);
	}

	for (const FAssetData& FoundData : InActorData)
	{
		InitializeSavedActor_Internal(PlaysetRoot->GetActorLocation(), FoundData);
	}

	MarkPackageDirty();
}

void UPlayset::InitializeSavedActor_Internal(const FVector& RelativeOrigin, const FAssetData& InActorData)
{
	AActor* Actor = Cast<AActor>(InActorData.GetAsset());
	if (!ensure(Actor))
	{
		return;
	}

	/*FArchive Ar;
	Actor->Serialize(Ar);
	UE_LOG(LogTemp, Warning, TEXT("Actor: %s was serialized. (Ar: %s)"), *Actor->GetName(), *Ar.GetArchiveName());*/

	/*FArchiveUObject Ar;
	Ar.ArContainsCode = true;

	Actor->Serialize(Ar);
	UE_LOG(LogTemp, Warning, TEXT("Actor: %s was serialized. (Ar: %s)"), *Actor->GetName(), *Ar.GetArchiveName());*/

	// Get all the modified properties of the actor
	/*FPropertyPairsMap PropMap;
	Actor->GetActorDescProperties(PropMap);

	auto ForEachProp = [](FName Key, FName Val)
	{
		UE_LOG(LogTemp, Warning, TEXT("Key: %s, Val: %s"), *Key.ToString(), *Val.ToString());
	};

	PropMap.ForEachProperty(ForEachProp);*/
	
	const UClass* ActorClass = Actor->GetClass();
	int32& Count = ActorClassCount.FindOrAdd(ActorClass);
	Count++;

	auto GetActorTemplateId = [&] (const FPlaysetActorData& Data)
	{
		int32 Id = INDEX_NONE;
		for (const auto& ExistingData : ActorData)
		{
			if (Data.ActorClass->GetPathName() != ExistingData.ActorClass->GetPathName())
			{
				continue;
			}

			++Id;
		}

		return Id;
	};

	FPlaysetActorData& NewActorData = ActorData.AddDefaulted_GetRef();
	NewActorData.ActorClass = ActorClass;
	NewActorData.RelativeLocation = Actor->GetActorLocation() - RelativeOrigin;
	NewActorData.RelativeRotation = Actor->GetActorRotation();
	NewActorData.RelativeScale = Actor->GetActorScale3D();
	NewActorData.ActorTemplateID = GetActorTemplateId(NewActorData);
	NewActorData.CollectActorName();
}
#endif


#if WITH_EDITOR
void UPlayset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	for (FPlaysetActorData& PreviewActorData : ActorData)
	{
		PreviewActorData.CollectActorName();
	}
}

void UPlayset::PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext)
{
	Super::PostSaveRoot(ObjectSaveContext);

	for (FPlaysetActorData& PreviewActorData : ActorData)
	{
		PreviewActorData.CollectActorName();
	}
}

EDataValidationResult UPlayset::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	// Make sure the playset level range is valid
	if (DataList.MinLevel > DataList.MaxLevel)
	{
		Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
		Context.AddError(LOCTEXT("MinLevelGreaterThanMaxLevel", "The minimum level of this playset must be less than or equal to the maximum level."));
	}

	// Make sure the preview actors data is valid
	for (const FPlaysetActorData& PreviewActorData : ActorData)
	{
		if (PreviewActorData.ActorClass == nullptr)
		{
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
			Context.AddError(LOCTEXT("InvalidActorClass", "The actor class of the preview actor data must be valid."));
		}
	}

	// Make sure the actor class count is valid
	for (const TPair<TSoftClassPtr<AActor>, int32>& ActorClassCountPair : ActorClassCount)
	{
		if (ActorClassCountPair.Key.IsNull())
		{
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
			Context.AddError(LOCTEXT("InvalidActorClass", "The actor class of the actor class count must be valid."));
		}

		if (ActorClassCountPair.Value < 0)
		{
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
			Context.AddError(LOCTEXT("InvalidActorClassCount", "The actor class count must be greater than or equal to zero."));
		}
	}

	return Result;
}
#endif

#undef LOCTEXT_NAMESPACE
