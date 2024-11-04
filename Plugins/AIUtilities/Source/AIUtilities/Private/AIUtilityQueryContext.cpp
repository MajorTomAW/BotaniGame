// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AIUtilityQueryContext.h"

float FAIUtilityParameterContainer::GetFloatValue(const FName Key)
{
	return 0.f;
}

int32 FAIUtilityParameterContainer::GetIntValue(const FName Key)
{
	return 0;
}

bool FAIUtilityParameterContainer::GetBoolValue(const FName Key)
{
	return 0;
}

FName FAIUtilityParameterContainer::GetNameValue(const FName Key)
{
	return FName();
}

FVector FAIUtilityParameterContainer::GetVectorValue(const FName Key)
{
	return FVector::ZeroVector;
}

FGameplayTag FAIUtilityParameterContainer::GetGameplayTagValue(const FName Key)
{
	return FGameplayTag();
}

FGameplayTagContainer FAIUtilityParameterContainer::GetGameplayTagContainerValue(const FName Key)
{
	return FGameplayTagContainer();
}
