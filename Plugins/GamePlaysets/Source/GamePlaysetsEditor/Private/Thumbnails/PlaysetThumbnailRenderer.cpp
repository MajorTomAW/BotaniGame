// Copyright © 2024 MajorT. All rights reserved.

#include "PlaysetThumbnailRenderer.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "ObjectTools.h"
#include "Playset.h"
#include "Settings/PlaysetDeveloperSettings.h"
#include "Slate/SlateBrushAsset.h"

bool UPlaysetThumbnailRenderer::CanVisualizeAsset(UObject* Object)
{
	const bool bCanDraw = (GetThumbnailTextureFromObject(Object) != nullptr);

	if (!bCanDraw)
	{
		ClearThumbnailCache(Object);
	}

	return bCanDraw;
}

void UPlaysetThumbnailRenderer::GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const
{
	Super::GetThumbnailSize(GetThumbnailTextureFromObject(Object), Zoom, OutWidth, OutHeight);
}

void UPlaysetThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* Viewport, FCanvas* Canvas, bool bAdditionalViewFamily)
{
	UTexture2D* ThumbnailImage = GetThumbnailTextureFromObject(Object);
	if (ThumbnailImage == nullptr)
	{
		ClearThumbnailCache(Object);
		return;
	}

	/*USlateBrushAsset* BrushAsset = UPlaysetDeveloperSettings::Get()->DefaultBackgroundBrush.LoadSynchronous();
	if (BrushAsset)
	{
		if (UMaterialInstance* MaterialInstance = Cast<UMaterialInstance>(BrushAsset->Brush.GetResourceObject()))
		{
			if (UMaterialInstanceDynamic* MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(MaterialInstance, nullptr))
			{
				MaterialInstanceDynamic->SetTextureParameterValue("Texture", ThumbnailImage);
			}
		}

		Super::Draw(BrushAsset->Brush.GetResourceObject(), X, Y, Width, Height, Viewport, Canvas, bAdditionalViewFamily);
	}
	else
	{
		Super::Draw(ThumbnailImage, X, Y, Width, Height, Viewport, Canvas, bAdditionalViewFamily);	
	}*/

	Super::Draw(ThumbnailImage, X, Y, Width, Height, Viewport, Canvas, bAdditionalViewFamily);	
}

void UPlaysetThumbnailRenderer::ClearThumbnailCache(UObject* Object)
{
	const UPlayset* Playset = Cast<UPlayset>(Object);
	check(Playset);

	UPackage* Package = Playset->GetOutermost();
	ThumbnailTools::CacheEmptyThumbnail(Playset->GetFullName(), Package);
}

UTexture2D* UPlaysetThumbnailRenderer::GetThumbnailTextureFromObject(UObject* Object) const
{
	UPlayset* Playset = Cast<UPlayset>(Object);
	if (Playset && Playset->DataList.GetThumbnailImage())
	{
		UTexture2D* ThumbnailImage = Playset->DataList.GetThumbnailImage();
		
		// Avoid calling UpdateResource on cooked texture as doing so will destroy the texture's data
		if (!Playset->GetPackage()->HasAnyPackageFlags(PKG_Cooked | PKG_FilterEditorOnly))
		{
			ThumbnailImage->FinishCachePlatformData();
			ThumbnailImage->UpdateResource();
		}
		return ThumbnailImage;
	}

	return nullptr;
}
