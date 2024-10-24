// Copyright © 2024 MajorT. All rights reserved.


#include "Libraries/ConversionsFunctionLibrary.h"

UConversionsFunctionLibrary::UConversionsFunctionLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

float UConversionsFunctionLibrary::GetShortestAngle(float from, float to)
{
	float difference = fmod(abs(to - from), 360.f);
	float distance = difference > 180.f ? 360.f - difference : difference;

	float sign = ((to - from >= 0 && to - from <= 180) || (to - from <=-180 && to - from>= -360)) ? 1.f : -1.f;
	return sign * distance;
}
