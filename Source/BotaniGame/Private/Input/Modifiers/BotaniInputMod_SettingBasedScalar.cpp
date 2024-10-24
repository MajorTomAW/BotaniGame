// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Input/Modifiers/BotaniInputMod_SettingBasedScalar.h"

#include "Input/Modifiers/BotaniInputModHelpers.h"
#include "Player/BotaniLocalPlayer.h"
#include "Settings/BotaniSettingsShared.h"

FInputActionValue UBotaniInputMod_SettingBasedScalar::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	if (!ensureMsgf(CurrentValue.GetValueType() != EInputActionValueType::Boolean, TEXT("Setting Based Scalar modifier doesn't support boolean values.")))
	{
		return CurrentValue;
	}

	UBotaniLocalPlayer* LocalPlayer = BotaniInputModHelpers::GetLocalPlayer(PlayerInput);
	if (!LocalPlayer)
	{
		return CurrentValue;
	}

	const UClass* SettingsClass = UBotaniSettingsShared::StaticClass();
	UBotaniSettingsShared* SharedSettings = LocalPlayer->GetSharedSettings();

	const bool bHasCachedProperty = PropertyCache.Num() == 3;
	
	const FProperty* XAxisValue = bHasCachedProperty ? PropertyCache[0] : SettingsClass->FindPropertyByName(XAxisScalarSettingName);
	const FProperty* YAxisValue = bHasCachedProperty ? PropertyCache[1] : SettingsClass->FindPropertyByName(YAxisScalarSettingName);
	const FProperty* ZAxisValue = bHasCachedProperty ? PropertyCache[2] : SettingsClass->FindPropertyByName(ZAxisScalarSettingName);

	if (PropertyCache.IsEmpty())
	{
		PropertyCache.Emplace(XAxisValue);
		PropertyCache.Emplace(YAxisValue);
		PropertyCache.Emplace(ZAxisValue);
	}

	FVector ScalarToUse = FVector::OneVector;

	switch (CurrentValue.GetValueType())
	{
	case EInputActionValueType::Axis3D:
		{
			ScalarToUse.Z = ZAxisValue ? *ZAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0f;
		}
	case EInputActionValueType::Axis2D:
		{
			ScalarToUse.Y = YAxisValue ? *YAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0f;
		}
	case EInputActionValueType::Axis1D:
		{
			ScalarToUse.X = XAxisValue ? *XAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0f;
		}
	default: break;
	}

	ScalarToUse.X = FMath::Clamp(ScalarToUse.X, MinValueClamp.X, MaxValueClamp.X);
	ScalarToUse.Y = FMath::Clamp(ScalarToUse.Y, MinValueClamp.Y, MaxValueClamp.Y);
	ScalarToUse.Z = FMath::Clamp(ScalarToUse.Z, MinValueClamp.Z, MaxValueClamp.Z);
			
	return CurrentValue.Get<FVector>() * ScalarToUse;
}
