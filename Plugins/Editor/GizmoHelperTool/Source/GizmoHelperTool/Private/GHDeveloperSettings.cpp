// Copyright © 2024 Game Shifters Studio. All rights reserved.


#include "GHDeveloperSettings.h"

#include "GHDataDefinitions.h"
#include "Engine/Light.h"
#include "GameFramework/Volume.h"
#include "Engine/TriggerBase.h"
#include "GameFramework/PlayerStart.h"

UGHDeveloperSettings::UGHDeveloperSettings()
{
	// Settings Setup
	SetDefaultValues();
	SetKeybindings();
	
	InternalVirtualActors.Add(ATriggerBase::StaticClass());
	InternalVirtualActors.Add(AVolume::StaticClass());
	InternalVirtualActors.Add(APlayerStart::StaticClass());
	InternalVirtualActors.Add(UBlueprintGeneratedClass::StaticClass());
}

UGHDeveloperSettings* UGHDeveloperSettings::Get()
{
	return GetMutableDefault<UGHDeveloperSettings>();
}

TArray<FKey> UGHDeveloperSettings::GetAllInputKeys()
{
	TArray<FKey> Keys;
	Keys.Add(Get()->MoveToMouseKey);
	Keys.Add(Get()->MoveToMouseKeyWithoutCollision);
	return Keys;
}

FName UGHDeveloperSettings::GetCategoryName() const
{
	return "Plugins";
}

void UGHDeveloperSettings::SetDefaultValues()
{
	CollisionSelection = EGizmoCollisionSelection::ComplexCollision;
	bPlaceActorsAtCameraIfNoHit = true;
	FallbackCustomBounds = FBoxSphereBounds(FVector::ZeroVector, FVector(5.f), 0.f);
	CustomBounds.Add(FGHActorCustomBounds(ALight::StaticClass(), FBoxSphereBounds(FVector::ZeroVector, FVector(50.f), 0.f)));
}

void UGHDeveloperSettings::SetKeybindings()
{
	MoveToMouseKey = EKeys::C;
	MoveToMouseKeyWithoutCollision = EKeys::X;
}

bool UGHDeveloperSettings::TraceToComplexCollision() const
{
	return CollisionSelection == EGizmoCollisionSelection::ComplexCollision;
}
