// Copyright © 2024 Botanibots Team. All rights reserved.


#include "BotaniPawnData_Actions.h"

#include "GameFeatures/Data/BotaniPawnData.h"
#define LOCTEXT_NAMESPACE "FBotaniPawnData_Actions"

FBotaniPawnData_Actions::FBotaniPawnData_Actions()
{
}

FText FBotaniPawnData_Actions::GetName() const
{
	return LOCTEXT("BotaniPawnData", "Pawn Data");
}

const TArray<FText>& FBotaniPawnData_Actions::GetSubMenus() const
{
	return PawnSubMenus;
}

UClass* FBotaniPawnData_Actions::GetSupportedClass() const
{
	return UBotaniPawnData::StaticClass();
}
#undef LOCTEXT_NAMESPACE
