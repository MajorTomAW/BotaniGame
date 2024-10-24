// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Camera/BotaniCameraMode.h"

#include "Camera/BotaniCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Canvas.h"
#include "GameFramework/Character.h"

//////////////////////////////////////////////////////////////////////////
/// FBotaniCameraModeView

FBotaniCameraModeView::FBotaniCameraModeView()
	: Location(ForceInit)
	, Rotation(ForceInit)
	, ControlRotation(ForceInit)
	, FieldOfView(BOTANI_CAMERA_DEFAULT_FOV)
{
}

void FBotaniCameraModeView::Blend(const FBotaniCameraModeView& Other, float OtherWeight)
{
	if (OtherWeight <= 0.f)
	{
		return;
	}

	if (OtherWeight >= 1.f)
	{
		*this = Other;
		return;
	}

	Location = FMath::Lerp(Location, Other.Location, OtherWeight);

	const FRotator DeltaRot = (Other.Rotation - Rotation).GetNormalized();
	Rotation = Rotation + DeltaRot * OtherWeight;

	const FRotator DeltaControlRot = (Other.ControlRotation - ControlRotation).GetNormalized();
	ControlRotation = ControlRotation + DeltaControlRot * OtherWeight;

	FieldOfView = FMath::Lerp(FieldOfView, Other.FieldOfView, OtherWeight);
}

//////////////////////////////////////////////////////////////////////////
/// UBotaniCameraMode

UBotaniCameraMode::UBotaniCameraMode()
	: FieldOfView(BOTANI_CAMERA_DEFAULT_FOV)
	, ViewPitchMin(BOTANI_CAMERA_DEFAULT_PITCH_MIN)
	, ViewPitchMax(BOTANI_CAMERA_DEFAULT_PITCH_MAX)
	, BlendTime(0.5f)
	, BlendFunction(EBotaniCameraModeBlendFunction::EaseOut)
	, BlendExponent(4.f)
	, BlendAlpha(1.f)
	, BlendWeight(1.f)
	, bResetInterpolation(0)
{
}

UBotaniCameraComponent* UBotaniCameraMode::GetBotaniCameraComponent() const
{
	return CastChecked<UBotaniCameraComponent>(GetOuter());
}

AActor* UBotaniCameraMode::GetTargetActor() const
{
	const UBotaniCameraComponent* CameraComponent = GetBotaniCameraComponent();
	return CameraComponent->GetTargetActor();
}

UWorld* UBotaniCameraMode::GetWorld() const
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		return GetOuter()->GetWorld();
	}

	return nullptr;
}

void UBotaniCameraMode::UpdateCameraMode(float DeltaTime)
{
	UpdateView(DeltaTime);
	UpdateBlending(DeltaTime);
}

void UBotaniCameraMode::SetBlendWeight(float InBlendWeight)
{
	BlendWeight = FMath::Clamp(InBlendWeight, 0.f, 1.f);

	// Since we're setting the blend weight directly, we should reset the interpolation.
	const float InvExponent = (BlendExponent > 0.f) ? (1.f / BlendExponent) : 1.f;

	switch (BlendFunction) {
	case EBotaniCameraModeBlendFunction::Linear:
		BlendAlpha = BlendWeight;
		break;
	case EBotaniCameraModeBlendFunction::EaseIn:
		BlendAlpha = FMath::InterpEaseIn(0.f, 1.f, BlendWeight, InvExponent);
		break;
	case EBotaniCameraModeBlendFunction::EaseOut:
		BlendAlpha = FMath::InterpEaseOut(0.f, 1.f, BlendWeight, InvExponent);
		break;
	case EBotaniCameraModeBlendFunction::EaseInOut:
		BlendAlpha = FMath::InterpEaseInOut(0.f, 1.f, BlendWeight, InvExponent);
		break;
	default:
		checkf(false, TEXT("Unknown blend function"));
	}
}

void UBotaniCameraMode::DrawDebug(UCanvas* Canvas) const
{
	check(Canvas);
	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetDrawColor(FColor::White);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("		BotaniCameraMode: %s (%f)"), *GetName(), BlendWeight));
}

FVector UBotaniCameraMode::GetPivotLocation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		// Height adjustments for characters to account for crouching.
		if (const ACharacter* TargetCharacter = Cast<ACharacter>(TargetPawn))
		{
			const ACharacter* TargetCharacterCDO = TargetCharacter->GetClass()->GetDefaultObject<ACharacter>();
			check(TargetCharacterCDO);

			const UCapsuleComponent* CapsuleComp = TargetCharacter->GetCapsuleComponent();
			check(CapsuleComp);

			const UCapsuleComponent* CapsuleCompCDO = TargetCharacterCDO->GetCapsuleComponent();
			check(CapsuleCompCDO);

			const float DefaultHalfHeight = CapsuleCompCDO->GetUnscaledCapsuleHalfHeight();
			const float ActualHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
			const float HeightAdjustment = (DefaultHalfHeight - ActualHalfHeight) + TargetCharacterCDO->BaseEyeHeight;

			return TargetCharacter->GetActorLocation() + (FVector::UpVector * HeightAdjustment);
		}

		return TargetPawn->GetPawnViewLocation();
	}

	return TargetActor->GetActorLocation();
}

FRotator UBotaniCameraMode::GetPivotRotation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		return TargetPawn->GetViewRotation();
	}

	return TargetActor->GetActorRotation();
}

void UBotaniCameraMode::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = PivotRotation;
	View.FieldOfView = FieldOfView;
}

void UBotaniCameraMode::UpdateBlending(float DeltaTime)
{
	if (BlendTime > 0.f)
	{
		BlendAlpha += (DeltaTime / BlendTime);
		BlendAlpha = FMath::Min(BlendAlpha, 1.f);
	}
	else
	{
		BlendAlpha = 1.f;
	}

	const float Exponent = (BlendExponent > 0.f) ? BlendExponent : 1.f;

	switch (BlendFunction)
	{
	case EBotaniCameraModeBlendFunction::Linear:
		BlendWeight = BlendAlpha;
		break;
	case EBotaniCameraModeBlendFunction::EaseIn:
		BlendWeight = FMath::InterpEaseIn(0.f, 1.f, BlendAlpha, Exponent);
		break;
	case EBotaniCameraModeBlendFunction::EaseOut:
		BlendWeight = FMath::InterpEaseOut(0.f, 1.f, BlendAlpha, Exponent);
		break;
	case EBotaniCameraModeBlendFunction::EaseInOut:
		BlendWeight = FMath::InterpEaseInOut(0.f, 1.f, BlendAlpha, Exponent);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
/// UBotaniCameraModeStack

UBotaniCameraModeStack::UBotaniCameraModeStack()
	: bIsActive(true)
{
}

void UBotaniCameraModeStack::ActivateStack()
{
	if (bIsActive)
	{
		return;
	}

	bIsActive = true;

	// Notify the camera modes that they're being activated.
	for (UBotaniCameraMode* CameraMode : CameraModeStack)
	{
		check(CameraMode);
		CameraMode->OnActivated();
	}
}

void UBotaniCameraModeStack::DeactivateStack()
{
	if (!bIsActive)
	{
		return;
	}

	bIsActive = false;

	// Notify the camera modes that they're being deactivated.
	for (UBotaniCameraMode* CameraMode : CameraModeStack)
	{
		check(CameraMode);
		CameraMode->OnDeactivated();
	}
}

void UBotaniCameraModeStack::PushCameraMode(TSubclassOf<UBotaniCameraMode> CameraModeClass)
{
	if (!CameraModeClass)
	{
		return;
	}

	UBotaniCameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);
	check(CameraMode);

	int32 StackSize = CameraModeStack.Num();

	if ((StackSize > 0) && (CameraModeStack[0] == CameraMode))
	{
		// The camera mode is already at the top of the stack.
		return;
	}

	int32 ExistingStackIndex = INDEX_NONE;
	float ExistingStackContribution = 1.f;

	for (int32 StackIdx = 0; StackIdx < StackSize; ++StackIdx)
	{
		if (CameraModeStack[StackIdx] == CameraMode)
		{
			ExistingStackIndex = StackIdx;
			ExistingStackContribution *= CameraMode->GetBlendWeight();
			break;
		}

		ExistingStackContribution *= (1.f - CameraModeStack[StackIdx]->GetBlendWeight());
	}

	if (ExistingStackIndex != INDEX_NONE)
	{
		CameraModeStack.RemoveAt(ExistingStackIndex);
		StackSize--;
	}
	else
	{
		ExistingStackContribution = 0.f;
	}

	// Decide what initial weight to start with.
	const bool bShouldBlend = ((CameraMode->GetBlendTime() > 0.f) && (StackSize > 0));
	const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.f);
	CameraMode->SetBlendWeight(BlendWeight);

	// Add a new entry on top of the stack.
	CameraModeStack.Insert(CameraMode, 0);

	// Make sure the stack bottom is always weighted 100%
	CameraModeStack.Last()->SetBlendWeight(1.f);

	// Let the camera mode know if its being added to the stack
	if (ExistingStackIndex == INDEX_NONE)
	{
		CameraMode->OnActivated();
	}
}

bool UBotaniCameraModeStack::EvaluateStack(float DeltaTime, FBotaniCameraModeView& OutView)
{
	if (!bIsActive)
	{
		return false;
	}

	UpdateStack(DeltaTime);
	BlendStack(OutView);
	return true;
}

void UBotaniCameraModeStack::DrawDebug(UCanvas* Canvas) const
{
	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetDrawColor(FColor::Green);
	DisplayDebugManager.DrawString(FString(TEXT("   --- Camera Modes (Begin) ---")));

	for (const UBotaniCameraMode* CameraMode : CameraModeStack)
	{
		check(CameraMode);
		CameraMode->DrawDebug(Canvas);
	}

	DisplayDebugManager.SetDrawColor(FColor::Green);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("   --- Camera Modes (End) ---")));
}

void UBotaniCameraModeStack::GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const
{
	if (CameraModeStack.Num() == 0)
	{
		OutWeightOfTopLayer = 1.0f;
		OutTagOfTopLayer = FGameplayTag();
		return;
	}
	
	const UBotaniCameraMode* TopEntry = CameraModeStack.Last();
	check(TopEntry);
	OutWeightOfTopLayer = TopEntry->GetBlendWeight();
	OutTagOfTopLayer = TopEntry->GetCameraModeTag();
}

UBotaniCameraMode* UBotaniCameraModeStack::GetCameraModeInstance(TSubclassOf<UBotaniCameraMode> CameraModeClass)
{
	check(CameraModeClass);

	// First see if we already have an instance of this camera mode.
	for (UBotaniCameraMode* CameraMode : CameraModeInstances)
	{
		if ((CameraMode != nullptr) && (CameraMode->GetClass() == CameraModeClass))
		{
			return CameraMode;
		}
	}

	// Not found, create a new instance.
	UBotaniCameraMode* NewCameraMode = NewObject<UBotaniCameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
	check(NewCameraMode);

	CameraModeInstances.Add(NewCameraMode);
	return NewCameraMode;
}

void UBotaniCameraModeStack::UpdateStack(float DeltaTime)
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	int32 RemoveCount = 0;
	int32 RemoveIndex = INDEX_NONE;

	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		UBotaniCameraMode* CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		CameraMode->UpdateCameraMode(DeltaTime);

		if (CameraMode->GetBlendWeight() >= 1.0f)
		{
			// Everything below this mode is now irrelevant and can be removed.
			RemoveIndex = (StackIndex + 1);
			RemoveCount = (StackSize - RemoveIndex);
			break;
		}
	}

	if (RemoveCount > 0)
	{
		// Let the camera modes know they being removed from the stack.
		for (int32 StackIndex = RemoveIndex; StackIndex < StackSize; ++StackIndex)
		{
			UBotaniCameraMode* CameraMode = CameraModeStack[StackIndex];
			check(CameraMode);

			CameraMode->OnDeactivated();
		}

		CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
	}
}

void UBotaniCameraModeStack::BlendStack(FBotaniCameraModeView& OutView) const
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	// Start with the bottom of the stack.
	const UBotaniCameraMode* BottomCameraMode = CameraModeStack[StackSize -1];
	check(BottomCameraMode);

	OutView = BottomCameraMode->GetCameraModeView();

	for (int32 StackIdx = (StackSize - 2); StackIdx >= 0; --StackIdx)
	{
		BottomCameraMode = CameraModeStack[StackIdx];
		check(BottomCameraMode);

		OutView.Blend(BottomCameraMode->GetCameraModeView(), BottomCameraMode->GetBlendWeight());
	}
}
