// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyleMacros.h"

class FBotaniEditorSlateStyle final : public FSlateStyleSet
{
public:
	FBotaniEditorSlateStyle()
		: FSlateStyleSet("BotaniEditorSlateStyle")
	{
		SetParentStyleName(FAppStyle::GetAppStyleSetName());
		FSlateStyleSet::SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate/Starship/AssetIcons"));
		FSlateStyleSet::SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

		static const FVector2D Icon16 = FVector2D(16.0f, 16.0f);
		static const FVector2D Icon64 = FVector2D(64.0f, 64.0f);

		Set("ClassIcon.BotaniPawnData", new IMAGE_BRUSH_SVG("Object_16", Icon16));
		Set("ClassThumbnail.BotaniPawnData", new IMAGE_BRUSH_SVG("Object_64", Icon64));

		Set("ClassIcon.BotaniGameData", new IMAGE_BRUSH_SVG("Object_16", Icon16));
		Set("ClassThumbnail.BotaniGameData", new IMAGE_BRUSH_SVG("Object_64", Icon64));

		Set("ClassIcon.BotaniCameraMode", new IMAGE_BRUSH_SVG("CameraActor_16", Icon16));
		Set("ClassThumbnail.BotaniCameraMode", new IMAGE_BRUSH_SVG("CameraActor_64", Icon64));

		Set("ClassIcon.BotDecisionRuleSet", new IMAGE_BRUSH_SVG("UserDefinedEnum_16", Icon16));
		Set("ClassThumbnail.BotDecisionRuleSet", new IMAGE_BRUSH_SVG("UserDefinedEnum_64", Icon64));

		/*FSlateStyleSet::SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate/Icons/AssetIcons"));
		Set("ClassIcon.BotaniAbilitySet", new IMAGE_BRUSH("AbilitySystemComponent_16x", Icon16));
		Set("ClassThumbnail.BotaniAbilitySet", new IMAGE_BRUSH("AbilitySystemComponent_64x", Icon64));*/

		FSlateStyleSet::SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate/Graph/Icons"));
		Set("ClassIcon.BotaniBotDefinition", new IMAGE_BRUSH("Robot", Icon16));
		Set("ClassThumbnail.BotaniBotDefinition", new IMAGE_BRUSH("Robot", Icon64));
	}
};
