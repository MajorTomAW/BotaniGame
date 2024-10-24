// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#define LOCTEXT_NAMESPACE "ValidatorToolbarButton"
#include "Styling/SlateStyleRegistry.h"

//////////////////////////////////////////////////////////////////////////
/// FEditorValidatorStyle

class FEditorValidatorStyle
{
public:
	static void Initialize();
	static void Shutdown();
	
	static FName GetStyleSetName();
	static const ISlateStyle& Get();

private:
	static TSharedRef<FSlateStyleSet> Create();
	static TSharedPtr<FSlateStyleSet> StyleInstance;
};

//////////////////////////////////////////////////////////////////////////
/// FEditorValidatorStyle


TSharedPtr< FSlateStyleSet > FEditorValidatorStyle::StyleInstance = nullptr;

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( FPaths::EngineContentDir() / "Editor/Slate"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( FPaths::EngineContentDir() / "Editor/Slate"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( FPaths::EngineContentDir() / "Editor/Slate"/ RelativePath + TEXT(".png"), __VA_ARGS__ )

#define GAME_IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( FPaths::ProjectPluginsDir() / "EditorContentValidation/Resources/Slate"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define GAME_IMAGE_BRUSH_SVG( RelativePath, ... ) FSlateVectorImageBrush( FPaths::ProjectPluginsDir() / "EditorContentValidation/Resources/Slate"/ RelativePath + TEXT(".svg"), __VA_ARGS__ )

inline void FEditorValidatorStyle::Initialize()
{
	if (StyleInstance.IsValid())
	{
		return;
	}

	StyleInstance = Create();
	FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
}

inline void FEditorValidatorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

inline FName FEditorValidatorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("GameEditorStyle"));
	return StyleSetName;
}

inline const ISlateStyle& FEditorValidatorStyle::Get()
{
	return *StyleInstance;
}

inline TSharedRef<FSlateStyleSet> FEditorValidatorStyle::Create()
{
	TSharedRef<FSlateStyleSet> StyleRef = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	StyleRef->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate"));
	StyleRef->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	FSlateStyleSet& Style = StyleRef.Get();

	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);
	const FVector2D Icon64x64(64.0f, 64.0f);

	Style.Set("GameEditor.CheckContent", new GAME_IMAGE_BRUSH_SVG("Icons/CheckContent", Icon20x20));
	return StyleRef;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH

//////////////////////////////////////////////////////////////////////////
/// FValidatorToolbarButton

class FValidatorToolbarButton
{
public:
	static FToolMenuEntry GetValidatorToolbarButton(const FExecuteAction& InExecuteAction);

private:
	static bool HasNoPlayWorld();
};


inline FToolMenuEntry FValidatorToolbarButton::GetValidatorToolbarButton(const FExecuteAction& InExecuteAction)
{
	FToolMenuEntry ToolMenuEntry = FToolMenuEntry::InitToolBarButton(
		"CheckContent",
		FUIAction(InExecuteAction,
			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)),
		LOCTEXT("CheckContent", "Check Content"),
		LOCTEXT("CheckContent_ToolTip", "Runs the Content Validation job on all checked out assets to look for warnings and errors."),
		FSlateIcon(FEditorValidatorStyle::GetStyleSetName(), "GameEditor.CheckContent")
		);

	return ToolMenuEntry;
}

inline bool FValidatorToolbarButton::HasNoPlayWorld()
{
	return !GEditor->PlayWorld;
}

#undef LOCTEXT_NAMESPACE
