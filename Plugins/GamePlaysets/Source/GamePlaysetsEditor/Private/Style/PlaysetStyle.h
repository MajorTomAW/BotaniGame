// Copyright © 2024 MajorT. All rights reserved.

#pragma once

class FPlaysetStyle
{
public:
	/** Initializes this style's Slate style set, creating it if necessary */
	static void Initialize();

	/** Shuts down this style's Slate style set, releasing it if necessary */
	static void Shutdown();

	/** Returns the slate style set. */
	static TSharedPtr<ISlateStyle> Get();

	/** Returns the style set name */
	static FName GetStyleSetName();

	/** Returns the color used for playsets */
	static const FLinearColor& GetPlaysetColor();
	
private:
	static FString InContent(const FString& RelativePath, const ANSICHAR* Extension);
	static TSharedPtr<FSlateStyleSet> StyleSet;
};