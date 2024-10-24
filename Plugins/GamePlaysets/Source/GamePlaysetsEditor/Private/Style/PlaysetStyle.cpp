// Copyright © 2024 MajorT. All rights reserved.

#include "PlaysetStyle.h"
#include "Brushes/SlateBoxBrush.h"
#include "Styling/SlateStyleRegistry.h"
#include "Brushes/SlateImageBrush.h"
#include "Styling/SlateTypes.h"
#include "Misc/Paths.h"
#include "Styling/CoreStyle.h"
#include "Interfaces/IPluginManager.h"
#include "SlateOptMacros.h"
#include "Settings/PlaysetDeveloperSettings.h"
#include "Styling/SlateStyle.h"


#define IMAGE_PLUGIN_BRUSH( RelativePath, ... ) FSlateImageBrush( FPaperStyle::InContent( RelativePath, ".png" ), __VA_ARGS__ )
#define IMAGE_PLUGIN_BRUSH_SVG( RelativePath, ... ) FSlateVectorImageBrush( FPaperStyle::InContent( RelativePath, ".svg" ), __VA_ARGS__ )
#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BOX_BRUSH(RelativePath, ...) FSlateBoxBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define DEFAULT_FONT(...) FCoreStyle::GetDefaultFontStyle(__VA_ARGS__)

TSharedPtr<FSlateStyleSet> FPlaysetStyle::StyleSet = nullptr;

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FPlaysetStyle::Initialize()
{
	// Const icon sizes
	const FVector2D Icon8x8(8.0f, 8.0f);
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);
	const FVector2D Icon64x64(64.0f, 64.0f);
	const FVector2D Icon300x100(300.0f, 100.0f);

	// Only register once
	if (StyleSet.IsValid())
	{
		return;
	}

	StyleSet = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	StyleSet->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate"));
	StyleSet->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));
	
	StyleSet->SetContentRoot(IPluginManager::Get().FindPlugin("GamePlaysets")->GetBaseDir() / TEXT("Resources"));
	StyleSet->Set("PlaysetsBrowser.AllPlaysets", new IMAGE_BRUSH(TEXT("AllPlaysets"), Icon300x100));
	StyleSet->Set("PlaysetsBrowser.CustomPlayset", new IMAGE_BRUSH(TEXT("CustomPlayset"), Icon300x100));

	const FTextBlockStyle& NormalText = FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");


	// Registers this slate style set
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef IMAGE_PLUGIN_BRUSH
#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef DEFAULT_FONT

void FPlaysetStyle::Shutdown()
{
	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		ensure(StyleSet.IsUnique());
		StyleSet.Reset();
	}
}

TSharedPtr<ISlateStyle> FPlaysetStyle::Get()
{
	return StyleSet;
}

FName FPlaysetStyle::GetStyleSetName()
{
	static FName PlaysetStyleName(TEXT("PlaysetStyle"));
	return PlaysetStyleName;
}

const FLinearColor& FPlaysetStyle::GetPlaysetColor()
{
	return UPlaysetDeveloperSettings::Get()->PlaysetColor;
}

FString FPlaysetStyle::InContent(const FString& RelativePath, const ANSICHAR* Extension)
{
	static FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("GamePlaysets"))->GetContentDir();
	return (ContentDir / RelativePath) + Extension;
}
